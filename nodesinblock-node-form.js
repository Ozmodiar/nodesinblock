// $Id$

(function ($) {

Drupal.behaviors.nodesinblockFieldsetSummaries = {
  attach: function (context) {
    $('fieldset#edit-nodesinblock', context).drupalSetSummary(function(context) {
      var vals = [];

      // region
      var nib_region = $("select[name='nodesinblock_delta'] option:selected", context).text();
      vals.push(nib_region);

      // Render mode.
      if (nib_region != Drupal.t('None')) {
        vals.push($("select[name='nodesinblock_render'] option:selected", context).text());
      }
        
      return Drupal.checkPlain(vals.join(', '));
    });
  }
};

})(jQuery);