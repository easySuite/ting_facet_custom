<?php
/**
 * @file
 * ting_facet_custom.features.user_permission.inc
 */

/**
 * Implements hook_user_default_permissions().
 */
function ting_facet_custom_user_default_permissions() {
  $permissions = array();

  // Exported permission: 'configure ting custom facets'.
  $permissions['configure ting custom facets'] = array(
    'name' => 'configure ting custom facets',
    'roles' => array(
      'administrators' => 'administrators',
      'local administrator' => 'local administrator',
    ),
    'module' => 'ting_facet_custom',
  );

  return $permissions;
}
