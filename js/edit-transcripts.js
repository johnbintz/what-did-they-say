/**
 * Set up a transcript editor widget.
 */
WhatDidTheySay.setup_transcript_editor = function(container) {
  if (container) {
    container = $(container);

    var button_holder = container.select('.wdts-button-holder').pop();

    if (button_holder) {
      var language_selector = container.select('.wdts-transcript-selector').pop();

      var get_transcript;

      if (language_selector) {
        var switch_transcripts = function() {
          container.select('.edit-transcript').each(function(t) {
            (t.name == 'wdts[transcripts][' + $F(language_selector) + ']') ? t.show() : t.hide();
          });
        };

        switch_transcripts();

        language_selector.switch_transcripts = switch_transcripts;

        language_selector.observe('change', switch_transcripts);

        get_transcript = function() { return container.select('textarea[name*=[' + $F(language_selector) + ']]').pop(); };
      } else {
        get_transcript = function() { return container.select('textarea').pop(); }
      }

      [ 'scene-heading', 'scene-action', 'dialog' ].each(function(tag) {
        var b = new Element('button', { className: 'button' }).update(WhatDidTheySay.button_labels[tag]);

        var get_t = function() { return tag; };

        b.observe('click', function(e) {
          Event.stop(e);

          var transcript = get_transcript();

          if (transcript) {
            if (document.selection) {
              var range = document.selection.createRange();
              var stored_range = range.duplicate();
              stored_range.moveToElementText(transcript);
              stored_range.setEndPoint('EndToEnd', range);
              transcript.selectionStart = stored_range.text.length - range.text.length;
              transcript.selectionEnd = transcript.selectionStart + range.text.length;
            }

            var start = transcript.selectionStart;
            var end = transcript.selectionEnd;

            var injector = new WDTSInjector(transcript, end);

            var tag = get_t();

            var new_content = (start == end);
            switch (tag) {
              case 'scene-heading':
              case 'scene-action':
                var message = tag.replace('-', '_');
                if (new_content) {
                  var content = prompt(WhatDidTheySay.messages[message], '');
                  if (content) {
                    injector.inject('[' + tag + ']' + content + "[/" + tag + "]\n", start);
                  }
                } else {
                  injector.inject("[/" + tag + "]\n", end);
                  injector.inject('[' + tag + ']', start);
                }
                break;
              case 'dialog':
                var name = prompt(WhatDidTheySay.messages.dialog_name, '');
                if (name) {
                  var direction = prompt(WhatDidTheySay.messages.dialog_direction, '');
                  var tag = '[dialog name="' + name + '"';
                  if (direction) { tag += ' direction="' + direction + '"'; }
                  tag += ']';

                  if (new_content) {
                    var speech = prompt(WhatDidTheySay.messages.dialog_speech, '');

                    if (speech) {
                      tag += speech + "[/dialog]\n";

                      injector.inject(tag, start);
                    }
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

        button_holder.insert(b);
      });
    }

    // submit button handling
    var submit_button = container.select('.wdts-modify-transcript').pop();
    var update_message = container.select('.wdts-update-message').pop();
    var post_id = $(container.parentNode.parentNode).select("input[name*=[post_id]]").shift();

    if (submit_button && update_message && post_id) {
      post_id = post_id.value;
      submit_button.observe('click', function(e) {
        Event.stop(e);

        var parameters = {
          'wdts[_nonce]': WhatDidTheySay.nonce,
          'wdts[module]': 'manage-post-transcripts',
          'wdts[post_id]': post_id
        };

        container.select('textarea').each(function(t) {
          parameters[t.name] = t.value;
        });

        var update_update_message = function(message) {
          update_message.update(message);
          update_message.setOpacity(1);
          update_message.show();
          new Effect.Highlight(update_message, { endcolor: '#DFDFDF'} );

          new PeriodicalExecuter(function(pe) {
            pe.stop();
            new Effect.Fade(update_message, {
              from: 1, to: 0, duration: 0.25, afterFinish: function() { update_message.update("") }
            });
          }, 3);
        }

        new Ajax.Request(
          WhatDidTheySay.ajax_url, {
            'method': 'post',
            'parameters': parameters,
            'onSuccess': function() { update_update_message(WhatDidTheySay.messages.transcripts_updated); },
            'onFailure': function() { update_update_message(WhatDidTheySay.messages.transcripts_failure); }
          }
        );
      });
    }
  }
};

/**
 * Set up action buttons for queued transcripts.
 */
WhatDidTheySay.setup_transcript_action_buttons = function(container, approved_editor_container) {
  if (container) {
    container = $(container);

    if (approved_editor_container) {
      approved_editor_container = $(approved_editor_container);
    }

    var actions_holder = container.select('.queued-transcript-actions').pop();

    if (actions_holder) {
      [
        [ 'approve',
          function(e) {
            Event.stop(e);
            var lang = container.select("input[name*=[language]]").shift();
            var post_id = container.select("input[name*=[post_id]]").shift();
            var key = container.select("input[name*=[key]]").shift();
            if (lang && post_id && key) {
              lang = lang.value;
              post_id = post_id.value;
              key = key.value;

              var editor = approved_editor_container.select('textarea[name*=[' + lang + ']').pop();
 
              var raw_transcript = container.select(".queued-transcript-raw").shift();

              if (raw_transcript && editor) {
                var ok = true;
                if (editor.value.match(/[^ ]/)) {
                  ok = confirm(WhatDidTheySay.messages.overwrite);
                }
                if (ok) {
                  editor.value = raw_transcript.innerHTML;

                  new Ajax.Request(
                    WhatDidTheySay.ajax_url, {
                      'method': 'post',
                      'parameters': {
                        'wdts[_nonce]': WhatDidTheySay.nonce,
                        'wdts[module]': 'approve-transcript',
                        'wdts[key]': key,
                        'wdts[post_id]': post_id
                      },
                      'onSuccess': function() {
                        container.update(WhatDidTheySay.messages.approved);
                        new Effect.Highlight(container);
                        new PeriodicalExecuter(function(pe) {
                          new Effect.Fade(container, {
                            from: 1, to: 0, duration: 0.25, afterFinish: function() { container.hide(); }
                          });
                        }, 3);

                        var language_selector = approved_editor_container.select('select').pop();
                        if (language_selector) {
                          var i,il;
                          for (i = 0, il = language_selector.options.length; i < il; ++i) {
                            if (language_selector.options[i].value == lang) {
                              language_selector.selectedIndex = i;
                              language_selector.switch_transcripts();
                              
                              break;
                            }
                          }
                        }
                      }
                    }
                  );
                }
              }
            }
          }
        ],
        [
          'delete',
          function(e) {
            Event.stop(e);

            if (confirm(WhatDidTheySay.messages.delete_message)) {
              var post_id = container.select("input[name*=[post_id]]").pop();
              var key = container.select("input[name*=[key]]").pop();

              if (post_id && key) {
                post_id = post_id.value;
                key = key.value;

                new Ajax.Request(
                  WhatDidTheySay.ajax_url, {
                    'method': 'post',
                    'parameters': {
                      'wdts[_nonce]': WhatDidTheySay.nonce,
                      'wdts[module]': 'delete-transcript',
                      'wdts[key]': key,
                      'wdts[post_id]': post_id
                    },
                    'onSuccess': function() {
                      container.update(WhatDidTheySay.messages.deleted);
                      new Effect.Highlight(container);

                      new PeriodicalExecuter(function(pe) {
                        new Effect.Fade(container, { from: 1, to: 0, duration: 0.5 });
                        pe.stop();
                      }, 2);
                    }
                  }
                );
              }
            }
          }
        ],
        [
          'edit',
          function(e) {
            Event.stop(e);

            var transcript = container.select('.wdts-transcript').pop();

            var editor        = new Element("div", { className: 'wdts-transcript-editor' });
            var button_holder = new Element("div", { className: 'wdts-button-holder' });
            var textnode   = new Element('textarea', { style: 'height: 200px; width: 99%' });
            textnode.value = container.select('.queued-transcript-raw').pop().innerHTML;

            editor.appendChild(button_holder);
            editor.appendChild(textnode);

            container.insertBefore(editor, transcript);
            transcript.hide();

            WhatDidTheySay.setup_transcript_editor(editor);

            var post_id = container.select("input[name*=[post_id]]").shift();
            var key = container.select("input[name*=[key]]").shift();

            var submitter  = new Element('button').update('Update Transcript');
            submitter.observe('click', function(e) {
              Event.stop(e);
              if (post_id && key) {
                post_id = post_id.value;
                key = key.value;

                new Ajax.Updater(container, WhatDidTheySay.ajax_url, {
                  'method': 'post',
                  'parameters': {
                    'wdts[_nonce]': WhatDidTheySay.nonce,
                    'wdts[module]': 'update-queued-transcript',
                    'wdts[key]': key,
                    'wdts[post_id]': post_id,
                    'wdts[transcript]': textnode.value
                  },
                  'onComplete': function() {
                    new Effect.Highlight(container);
                    WhatDidTheySay.setup_transcript_action_buttons(container, approved_editor_container);
                  }
                });
              }
            });

            container.appendChild(submitter);

            actions_holder.parentNode.removeChild(actions_holder);
          }
        ]
      ].each(function(info) {
        var ok = true;
        if (info[0] == 'approve') { ok = approved_editor_container; }
        if (ok) {
          var button = new Element("button").update(WhatDidTheySay.button_labels[info[0]]);
          button.observe('click', info[1]);

          actions_holder.insert(button);
        }
      });
    }
  }
};

WhatDidTheySay.setup_allow_new_transcripts = function(checkbox, editor) {
  if (checkbox) {
    checkbox = $(checkbox);

    checkbox.observe('click', function(e) {
      if (editor) { editor = $(editor); }

      var p = $(checkbox.parentNode);
      if (p) {
        var post_id = p.select("input[name*=[post_id]]").pop();

        if (post_id) {
          post_id = post_id.value;

          var parameters = {
            'wdts[_nonce]': WhatDidTheySay.nonce,
            'wdts[module]': 'manage-post-transcripts',
            'wdts[post_id]': post_id
          };

          if (checkbox.checked) {
            parameters['wdts[allow_on_post]'] = checkbox.value;
          }

          new Ajax.Request(
            WhatDidTheySay.ajax_url, {
              'method': 'post',
              'parameters': parameters,
              'onSuccess': function() {
                if (editor) {
                  if (checkbox.checked) {
                    new Effect.BlindDown(editor, { duration: 0.5 });
                  } else {
                    new Effect.BlindUp(editor, { duration: 0.5 });
                  }
                }
              }
            }
          );
        }
      }
    });
  }
};

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
  $$('.wdts-updated').each(function(up) {
    up.hide();
    up.style.top = document.viewport.getScrollOffsets().top + "px";
    new Effect.BlindDown(up, {
      duration: 0.25,
      afterFinish: function() {
        new PeriodicalExecuter(function(pe) {
          pe.stop();
          new Effect.BlindUp(up, {
            duration: 0.25
          });
        }, 3);
      }
    });
  });
});