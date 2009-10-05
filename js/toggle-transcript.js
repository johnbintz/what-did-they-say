WhatDidTheySay.get_transcript_language_code = function(transcript) {
  for (code in WhatDidTheySay.languages) {
    if (WhatDidTheySay.languages.hasOwnProperty(code)) {
      if (transcript.hasClassName(code)) {
        return code;
      }
    }
  }
  return false
};

WhatDidTheySay.build_bundle_header = function(bundle) {
  bundle.select('h3').each(function(h) { h.parentNode.removeChild(h); });

  var new_header = new Element('h3', { 'className': 'wdts-transcript-language' });

  var transcript_holders = bundle.select('.transcript-holder');
  transcript_holders.invoke('show');

  var show_only_this_code = function(code) {
    transcript_holders.each(function(t) {
      if (t.hasClassName(code)) {
        t.show();
      } else {
        t.hide();
      }
    });
  };

  if (transcript_holders.length > 1) {
    var select = new Element('select');

    show_only_this_code(WhatDidTheySay.default_language);
    
    transcript_holders.each(function(t) {
      var code = WhatDidTheySay.get_transcript_language_code(t);
      
      if (code) {
        var option = new Element('option', { 'value': code }).update(WhatDidTheySay.languages[code]);
        if (code == WhatDidTheySay.default_language) { option.selected = true; }
        select.insert(option);
      }
    });

    select.observe('change', function(e) {
      var code = select.options[select.selectedIndex].value;
      show_only_this_code(code);
    });

    new_header.update(WhatDidTheySay.messages.bundle_header.replace('%s', '<span />'));
    new_header.select('span')[0].insert(select);
  } else {
    var code = WhatDidTheySay.get_transcript_language_code(transcript_holders[0]);

    if (code) {
      new_header.update(WhatDidTheySay.messages.bundle_header.replace('%s', WhatDidTheySay.languages[code]));
    }
  }

  bundle.insert({ top: new_header })
}

$$('.wdts-transcript-container').each(function(d) {
  var bundle = d.select('.wdts-transcript-bundle').pop();
  var opener_container = d.select('.wdts-transcript-opener').pop();

  if (bundle && opener_container) {
    WhatDidTheySay.build_bundle_header(bundle);

    var opener = opener_container.select('.wdts-opener').pop();
    var closer = opener_container.select('.wdts-closer').pop();

    if (opener && closer) {
      opener.observe('click', function(e) {
        opener.hide();
        closer.show();
        bundle.show();
      });

      closer.observe('click', function(e) {
        closer.hide();
        opener.show();
        bundle.hide();
      });
    }

    if (d.hasClassName('wdts-start-hidden')) {
      bundle.hide();
      closer.hide();
      opener.show();
    } else {
      bundle.show();
      closer.show();
      opener.hide();
    }
  }
});
