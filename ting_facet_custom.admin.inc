<?php
/**
 * @file
 * Admin related logic.
 */

/**
 * Custom facet administration form.
 */
function ting_facet_custom_admin_form($form, &$form_state) {
  $rows_new = isset($form_state['facet_rows']) ? $form_state['facet_rows'] : 1;
  $form_state['facet_rows'] = $rows_new;

  $form['fetch_facet_wrapper'] = array(
    '#type' => 'fieldset',
    '#title' => t('Fetch facet terms'),
  );

  $facets = variable_get('facets_wrapper', array());

  $form['fetch_facet_wrapper']['facet_identifier'] = array(
    '#type' => 'textfield',
    '#title' => t('Facet idenfitier'),
    '#default_value' => isset($facets['identifier']) ? $facets['identifier'] : 'department',
    '#required' => TRUE,
  );

  $form['fetch_facet_wrapper']['fetch_facet_submit'] = array(
    '#type' => 'submit',
    '#value' => t('Fetch terms'),
    '#ajax' => array(
      'callback' => '_ting_facet_custom_fetch_facet_callback',
      'method' => 'replace',
      'effect' => 'fade',
      'wrapper' => 'ting-facet-custom-admin-form',
    ),
    '#submit' => array('_ting_facet_custom_fetch_submit'),
    '#limit_validation_errors' => array(
      array('facet_identifier'),
    ),
  );


  $form['facet_name'] = array(
    '#type' => 'textfield',
    '#title' => t('Facet name'),
    '#required' => TRUE,
    '#default_value' => isset($facets['title']) ? $facets['title'] : '',
  );

  $form['facets_wrapper'] = array(
    '#type' => 'container',
    '#tree' => TRUE,
    '#theme' => 'ting_facet_custom_facet_element',
    '#prefix' => '<div id="ting-facet-custom-wrapper">',
    '#suffix' => '</div>',
  );

  if (isset($form_state['input']['facets_wrapper'])) {
    $form_state['input']['facets_wrapper'] = array_values($form_state['input']['facets_wrapper']);
  }

  $rows = isset($facets['facets']) ? $facets['facets'] : array();
  for ($i = 0; $i < count($rows); $i++) {
    $form['facets_wrapper'][] = ting_facet_custom_row_form($form_state, $rows[$i]);
  }

  for ($i = 0; $i < $rows_new; $i++) {
    $form['facets_wrapper'][] = ting_facet_custom_row_form($form_state);
  }

  $form['new'] = array(
    '#type' => 'submit',
    '#value' => t('Add option'),
    '#ajax' => array(
      'callback' => '_ting_facet_custom_row_callback',
      'effect' => 'fade',
      'method' => 'replace',
      'wrapper' => 'ting-facet-custom-wrapper',
    ),
    '#submit' => array('_ting_facet_custom_row_submit'),
    '#limit_validation_errors' => array(),
  );

  $form['submit'] = array(
    '#type' => 'submit',
    '#value' => t('Save configuration'),
  );

  $form['#prefix'] = '<div id="ting-facet-custom-admin-form">';
  $form['#suffix'] = '</div>';
  $form['#attached'] = array(
    'css' => array(
      drupal_get_path('module', 'ting_facet_custom') . '/css/ting_facet_custom.styles.css',
    ),
  );

  return $form;
}

/**
 * Custom validate handler for facets form.
 *
 * @see ting_facet_custom_admin_form()
 */
function ting_facet_custom_admin_form_validate($form, &$form_state) {
  $input = $form_state['input'];

  foreach ($input['facets_wrapper'] as $k => $v) {
    if (!empty($v['facet']) && empty($v['terms'])) {
      form_set_error('facets_wrapper][' . $k . '][terms', t('Select term(s) for the facet <em>@facet</em>.', array('@facet' => $v['facet'])));
    }
  }
}

/**
 * Custom submit handler for facets form.
 *
 * @see ting_facet_custom_admin_form()
 */
function ting_facet_custom_admin_form_submit($form, &$form_state) {
  $input = $form_state['input'];
  $facets = array_filter($input['facets_wrapper'], function($v) {
    return (!empty($v['facet']) && !empty($v['terms']));
  });

  $facets = array(
    'title' => trim($input['facet_name']),
    'identifier' => trim($input['facet_identifier']),
    'facets' => array_values($facets),
  );

  variable_set('facets_wrapper', $facets);
  drupal_set_message(t('Custom facets saved.'));
}

/**
 * Define single row form elements.
 *
 * @param array $form_state
 *   The form state.
 * @param array $default
 *   Default form elements values, if any.
 */
function ting_facet_custom_row_form(&$form_state, $default = array()) {
  $form = array();

  $form['weight'] = array(
    '#type' => 'weight',
    '#default_value' => isset($default['weight']) ? $default['weight'] : 0,
    '#attributes' => array(
      'class' => array('sort'),
    ),
  );

  $form['facet'] = array(
    '#type' => 'textfield',
    '#default_value' => isset($default['facet']) ? $default['facet'] : '',
  );

  $terms = variable_get('ting_facet_custom_terms', array());
  $form['terms'] = array(
    '#type' => 'select',
    '#options' => array_combine($terms, $terms),
    '#default_value' => isset($default['terms']) ? $default['terms'] : array(),
    '#multiple' => TRUE,
  );

  return $form;
}

/**
 * AJAX callback for new row button.
 *
 * @see ting_facet_custom_admin_form()
 */
function _ting_facet_custom_row_callback($form, &$form_state) {
  return $form['facets_wrapper'];
}

function _ting_facet_custom_fetch_facet_callback($form, &$form_state) {
  return $form;
}

/**
 * Custom submit handler for new row button.
 */
function _ting_facet_custom_row_submit($form, &$form_state) {
  $form_state['facet_rows']++;
  $form_state['rebuild'] = TRUE;
}

/**
 * Custom submit handler for fetching facet terms button.
 *
 * @see ting_facet_custom_admin_form()
 */
function _ting_facet_custom_fetch_submit($form, &$form_state) {
  $input = $form_state['input'];
  if (!empty($input['facet_identifier'])) {
    $terms = ting_facet_custom_facets('facet.' . $input['facet_identifier']);
  }

  $form_state['rebuild'] = TRUE;
}

/**
 * Custom theming handler for facet rows element.
 *
 * Used to render draggable rows in a table.
 *
 * @see ting_facet_custom_admin_form()
 * @see ting_facet_custom_theme()
 */
function theme_ting_facet_custom_facet_element($variables) {
  $element = $variables['element'];
  drupal_add_tabledrag('ting-facet-custom-table', 'order', 'sibling', 'sort');

  $header = array(
    t('Facet label'),
    t('Facet terms'),
    t('Weight'),
  );

  $rows = array();

  foreach (element_children($element) as $k => $v) {
    $rows[] = array(
      'data' => array(
        drupal_render($element[$k]['facet']),
        drupal_render($element[$k]['terms']),
        drupal_render($element[$k]['weight']),
      ),
      'class' => array('draggable'),
    );
  }

  $table = array(
    'header' => $header,
    'rows' => $rows,
    'attributes' => array(
      'id' => 'ting-facet-custom-table',
    ),
  );

  $output = theme('table', $table);
  $output .= drupal_render_children($element);


  return $output;
}
