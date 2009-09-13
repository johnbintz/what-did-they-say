function switch_transcript() {
  $$('.edit-transcript').each(function(t) {
    (t.id == "wdts-transcripts-" + $F('wdts-language')) ? t.show() : t.hide();
  });
}
switch_transcript();
Event.observe(language_selector, 'change', switch_transcript);

$$('.approve-transcript').each(function(b) {
  Event.observe(b, 'click', function(e) {
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