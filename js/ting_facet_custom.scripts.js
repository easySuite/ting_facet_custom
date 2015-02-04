/**
 * @file
 * Handle facet custom UI events.
 */
(function($) {
  Drupal.behaviors.ting_facet_custom = {
    attach : function(context, settings) {
      $('.ting-facet-custom-form .form-checkbox').click(function() {
        var href = $(this).parents('.form-type-checkbox').find('a').attr('href');
        Drupal.TingSearchOverlay();
        window.location = href;
      });

      $('.ting-facet-custom-form a').click(function() {
        Drupal.TingSearchOverlay();
      });
    }
  }
})(jQuery);
