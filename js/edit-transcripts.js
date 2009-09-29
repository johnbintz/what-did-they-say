var language_selector = $('wdts-language');

function switch_transcript() {
  $$('.edit-transcript').each(function(t) {
    (t.id == "wdts-transcripts-" + $F('wdts-language')) ? t.show() : t.hide();
  });
}

var WDTSInjector = Class.create({
  initialize: function(textarea, end) {
    this.textarea = textarea;
    this.end = end;
  },
  inject: function(text, position) {
    this.textarea.value = this.textarea.value.substr(0, position) + text + this.textarea.value.substr(position);
    this.end += text.length;
  },
  set_caret: function() {
    if (this.textarea.setSelectionRange) {
      this.textarea.focus();
      this.textarea.setSelectionRange(this.end, this.end);
    } else if (this.textarea.createTextRange) {
      var range = this.textarea.createTextRange();
      range.collapse(true);
      range.moveEnd('character', this.end);
      range.moveStart('character', this.end);
      range.select();
    }
  }
});

Event.observe(window, 'load', function() {
  var buttons_to_watch = [];
  if ($('wdts-transcript')) {
    buttons_to_watch.push([ '#wdts-submit-shorttags button', $('wdts-transcript') ]);
  }

  if ($$('select[name=wdts-language]').pop()) {
    buttons_to_watch.push([ '#wdts-shorttags button', $("wdts-transcripts-" + $F('wdts-language')) ]);
  }

  buttons_to_watch.each(function(info) {
    $$(info[0]).each(function(b) {
      b.observe('click', function(e) {
        Event.stop(e);
        var current_transcript = info[1];
        if (current_transcript) {
          if (document.selection) {
            var range = document.selection.createRange();
            var stored_range = range.duplicate();
            stored_range.moveToElementText(current_transcript);
            stored_range.setEndPoint('EndToEnd', range);
            current_transcript.selectionStart = stored_range.text.length - range.text.length;
            current_transcript.selectionEnd = current_transcript.selectionStart + range.text.length;
          }
          
          var start = current_transcript.selectionStart;
          var end = current_transcript.selectionEnd;

          var injector = new WDTSInjector(current_transcript, end);

          var new_content = (start == end);
          var tag = b.id.replace('wdts-', '');
          switch (b.id) {
            case 'wdts-scene-heading':
            case 'wdts-scene-action':
              var message = tag.replace('-', '_');
              if (new_content) {
                var content = prompt(messages[message]);
                if (content) {
                  injector.inject('[' + tag + ']' + content + "[/" + tag + "]\n", start);
                }
              } else {
                injector.inject("[/" + tag + "]\n", end);
                injector.inject('[' + tag + ']', start);
              }
              break;
            case 'wdts-dialog':
              var name = prompt(messages.dialog_name);
              if (name) {
                var direction = prompt(messages.dialog_direction);
                var tag = '[dialog name="' + name + '"';
                if (direction) { tag += ' direction="' + direction + '"'; }
                tag += ']';

                if (new_content) {
                  var speech = prompt(messages.dialog_speech);

                  tag += speech + "[/dialog]\n";

                  injector.inject(tag, start);
                } else {
                  injector.inject("[/dialog]\n", end);
                  injector.inject(tag, start);
                }
              }
              break;
          }
          injector.set_caret();
        }
      });
    });
  });
});

function wdts_setup_approve_transcript_clicker(b) {
  b.observe('click', function(e) {
    Event.stop(e);

    var p = $(b.parentNode.parentNode);
    
    var lang = p.select("input[name*=[language]]").shift();
    var post_id = p.select("input[name*=[post_id]]").shift();
    var key = p.select("input[name*=[key]]").shift();
    if (lang && post_id && key) {
      lang = lang.value;
      post_id = post_id.value;
      key = key.value;
      var editor = $('wdts-transcripts-' + lang);

      var raw_transcript = p.select(".queued-transcription-raw").shift();
      if (raw_transcript && editor) {
        var ok = true;
        if (editor.value.match(/[^ ]/)) {
          ok = confirm(messages.overwrite);
        }
        if (ok) {
          editor.value = raw_transcript.innerHTML;

          new Ajax.Request(
            ajax_url, {
              'method': 'post',
              'parameters': {
                'wdts[_nonce]': nonce,
                'wdts[module]': 'approve-transcript',
                'wdts[key]': key,
                'wdts[post_id]': post_id
              },
              'onSuccess': function() {
                p.update(messages.approved);
                new Effect.Highlight(p);
                var i,il;

                for (i = 0, il = language_selector.options.length; i < il; ++i) {
                  if (language_selector.options[i].value == lang) {
                    language_selector.selectedIndex = i;
                    switch_transcript();
                    break;
                  }
                }
              }
            }
          );
        }
      }
    }
  });
};

function wdts_setup_delete_transcript_clicker(b) {
  b.observe('click', function(e) {
    Event.stop(e);

    var p = $(b.parentNode.parentNode);
    
    if (confirm(messages.delete_message)) {
      var post_id = p.select("input[name*=[post_id]]").pop();
      var key = p.select("input[name*=[key]]").pop();

      if (post_id && key) {
        post_id = post_id.value;
        key = key.value;

        new Ajax.Request(
          ajax_url, {
            'method': 'post',
            'parameters': {
              'wdts[_nonce]': nonce,
              'wdts[module]': 'delete-transcript',
              'wdts[key]': key,
              'wdts[post_id]': post_id
            },
            'onSuccess': function() {
              p.update(messages.deleted);
              new Effect.Highlight(p);
            }
          }
        );
      }
    }
  });
}

function wdts_setup_edit_transcript_clicker(b) {
  b.observe('click', function(e) {
    Event.stop(e);

    var p = $(b.parentNode.parentNode);

    var transcript = p.select('.transcript').pop();
    var textnode   = new Element('textarea', { style: 'height: 200px; width: 90%' });
    var action_links = p.select('.transcript-action-links').pop();
    textnode.value = p.select('.queued-transcription-raw').pop().innerHTML;

    p.insertBefore(textnode, transcript);
    transcript.hide();

    var post_id = p.select("input[name*=[post_id]]").shift();
    var key = p.select("input[name*=[key]]").shift();

    var submitter  = new Element('button').update('Update Transcript');
    submitter.observe('click', function(e) {
      if (post_id && key) {
        post_id = post_id.value;
        key = key.value;

        new Ajax.Updater(p, ajax_url, {
          'method': 'post',
          'parameters': {
            'wdts[_nonce]': nonce,
            'wdts[module]': 'update-queued-transcript',
            'wdts[key]': key,
            'wdts[post_id]': post_id,
            'wdts[transcript]': textnode.value
          },
          'onComplete': function() {
            new Effect.Highlight(p);
            wdts_add_clickers(p);
          }
        });
      }
    });

    p.appendChild(submitter);
    action_links.parentNode.removeChild(action_links);
  });
}

function wdts_add_clickers(p) {
  p.select('.edit-transcript-button').each(function(b) { wdts_setup_edit_transcript_clicker(b); });
  p.select('.approve-transcript').each(function(b) { wdts_setup_approve_transcript_clicker(b); });
  p.select('.delete-transcript').each(function(b) { wdts_setup_delete_transcript_clicker(b); });
}

wdts_add_clickers($$('body').pop());
if (language_selector) {
  switch_transcript();
  Event.observe(window, 'load', switch_transcript);
  Event.observe(language_selector, 'change', switch_transcript);
}
