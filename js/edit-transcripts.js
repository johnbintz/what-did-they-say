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
  [
    [ '#wdts-shorttags button', $("wdts-transcripts-" + $F('wdts-language')) ],
    [ '#wdts-submit-shorttags button', $('wdts-transcript') ]
  ].each(function(info) {
    $$(info[0]).each(function(b) {
      b.observe('click', function(e) {
        Event.stop(e);
        var current_transcript = info[1];
        if (current_transcript) {
          if (document.selection) {
            var range = document.selection.createRange();
            var stored_range = range.duplicate();
            stored_range.moveToElementText( element );
            stored_range.setEndPoint( 'EndToEnd', range );
            element.selectionStart = stored_range.text.length - range.text.length;
            element.selectionEnd = element.selectionStart + range.text.length;
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

$$('.approve-transcript').each(function(b) {
  b.observe('click', function(e) {
    Event.stop(e);
    var lang = b.parentNode.parentNode.select("input[name*=[language]]").shift();
    var post_id = b.parentNode.parentNode.parentNode.select("input[name*=[post_id]]").shift();
    var key = b.parentNode.parentNode.select("input[name*=[key]]").shift();
    if (lang && post_id && key) {
      lang = lang.value;
      post_id = post_id.value;
      key = key.value;
      var editor = $('wdts-transcripts-' + lang);

      var raw_transcript = b.parentNode.parentNode.select(".queued-transcription-raw").shift();
      if (raw_transcript && editor) {
        var ok = true;
        if (editor.value.match(/[^ ]/)) {
          ok = confirm(messages.overwrite);
        }
        if (ok) {
          editor.value = raw_transcript.innerHTML;
          var p = b.parentNode.parentNode;
          
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
});

$$('.delete-transcript').each(function(b) {
  Event.observe(b, 'click', function(e) {
    Event.stop(e);

    if (confirm(messages.delete)) {
      var post_id = b.parentNode.parentNode.parentNode.select("input[name*=[post_id]]").shift();
      var key = b.parentNode.parentNode.select("input[name*=[key]]").shift();
      if (post_id && key) {
        post_id = post_id.value;
        key = key.value;
        var p = b.parentNode.parentNode;
        
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
});

switch_transcript();
Event.observe(window, 'load', switch_transcript);
Event.observe(language_selector, 'change', switch_transcript);
