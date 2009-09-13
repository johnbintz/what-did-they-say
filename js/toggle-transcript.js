$$('.transcript-bundle').each(function(d) {
  var select = d.select("select");
  if (select.length == 1) {
    select = select[0];
    var toggle_transcripts = function() {
      d.select(".transcript-holder").each(function(div) {              
        div.hasClassName($F(select)) ? div.show() : div.hide();
      }); 
    };
    Event.observe(select, 'change', toggle_transcripts);
    Event.observe(window, 'load', toggle_transcripts)
  }
});
