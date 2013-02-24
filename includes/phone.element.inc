<?php

/**
 * @file
 * Provides FAPI implementation for a phone element.
 */

/**
 * Implements hook_element_info().
 */
function _phone_element_info() {
  $path = drupal_get_path('module', 'phone');

  $types['phone'] = array(
    '#input' => TRUE,
    '#process' => array('phone_element_process'),
    '#element_validate' => array('phone_element_validate'),
    '#theme_wrappers' => array('form_element'),
    '#attached' => array(
      'css' => array($path . '/theme/phone.css'),
    ),
    '#phone_settings' => array(
      'number_size' => 15,
      'comment_allowed_values' => array(
        'home' => t('Home'),
        'work' => t('Work'),
        'mobile' => t('Mobile'),
        'fax' => t('Fax'),
      ),
      'comment_allowed_values_function' => '',
      'enable_comment' => TRUE,
      'comment_allowed_values_position' => 'before',
      'enable_extension' => FALSE,
      'extension_size' => 6,
      'enable_default_country' => TRUE,
      'default_country' => NULL,
      'all_country_codes' => TRUE,
      'country_codes' => array(
        'hide_single_cc' => FALSE,
        'country_selection' => array(),
      ),
      'country_code_position' => 'after',
    ),
  );

  return $types;
}

/**
 * Process an individual phone element.
 */
function phone_element_process($element, &$form_state, $form) {
  $item = $element['#value'];

  $settings = $element['#phone_settings'];

  if ($settings['enable_comment'] && !empty($settings['comment_allowed_values'])) {
    $element['comment'] = array(
      '#type' => 'select',
      '#options' => $settings['comment_allowed_values'],
      '#weight' => $settings['comment_allowed_values_position'] == 'after' ? 5 : -5,
      '#empty_option' => t('- Select -'),
      '#default_value' => isset($item['comment']) ? $item['comment'] : NULL,
    );
  }
  else {
    $element['comment'] = array(
      '#type' => 'hidden',
      '#value' => isset($item['comment']) ? $item['comment'] : NULL,
    );
  }

  $element['number'] = array(
    '#type' => 'textfield',
    '#maxlength' => $settings['number_size'],
    '#size' => $settings['number_size'],
    '#required' => ($element['#delta'] == 0 && $element['#required']) ? $element['#required'] : FALSE,
    '#default_value' => isset($item['number']) ? $item['number'] : NULL,
    '#attached' => array(
      'css' => array(
        drupal_get_path('module', 'phone') . '/theme/phone.css',
      )
    ),
    '#weight' => 0,
  );

  // If only one country code, make it as a hidden form item.
  $country_list = array_filter($settings['country_codes']['country_selection']);
  if (!$settings['all_country_codes'] && count($country_list) == 1) {
    $countrycode = reset($country_list);
    $country = phone_countries($countrycode);
    $element['countrycode'] = array(
      '#type' => 'hidden',
      '#value' => $countrycode,
    );
    if (!$settings['country_codes']['hide_single_cc']) {
      $element['country_code_markup'] = array(
        '#type' => 'item',
        '#markup' => $value = $country,
        '#weight' => ($settings['country_code_position'] == 'after' ? 1 : -1),
      );
    }
  }
  else {
    $element['countrycode'] = array(
      '#type' => 'select',
      '#options' => phone_countries($settings['all_country_codes'] ? NULL : array_filter($settings['country_codes']['country_selection'])),
      '#weight' => ($settings['country_code_position'] == 'after' ? 1 : -1),
      '#empty_option' => t('- Guess from number -'),
    );
    if (isset($item['countrycode'])) {
      $element['countrycode']['#default_value'] = $item['countrycode'];
    }
    elseif ($settings['enable_default_country']) {
      $element['countrycode']['#default_value'] = $settings['default_country'];
    }
  }

  if ($settings['enable_extension']) {
    $element['extension'] = array(
     '#type' => 'textfield',
     '#maxlength' => $settings['extension_size'],
     '#size' => $settings['extension_size'],
     '#title' => t('ext'),
     '#required' => FALSE,
     '#default_value' => isset($item['extension']) ? $item['extension'] : NULL,
     '#weight' => 2,
    );
  }
  else {
    $element['extension'] = array(
      '#type' => 'hidden',
      '#value' => isset($item['extension']) ? $item['extension'] : NULL,
    );
  }

  return $element;
}

/**
 * An #element_validate callback for the phone element.
 */
function phone_element_validate(&$element, &$form_state) {
  // Load up libphonenumer.
  phone_libphonenumber();

  $item = $element['#value'];
  if (isset($item['number'])) {
    $phone_input = trim($item['number']);
  }
  if (isset($item['countrycode'])) {
    $countrycode = trim($item['countrycode']);
  }
  $ext_input = '';
  $settings = $element['#phone_settings'];

  if ($settings['enable_extension'] && isset($item['extension'])) {
    $ext_input = trim($item['extension']);
  }

  if (isset($phone_input) && !empty($phone_input)) {
    $valid = phone_libphonenumber_valid($phone_input, $countrycode, $ext_input);
    if (TRUE !== $valid) {
      // We don't seem to get a title when this is used in a multi-valued field. Core bug?
      if (!empty($element['#title'])) {
        form_error($element, t('%name: !error', array('%name' => $element['#title'], '!error' => $valid)));
      }
      else {
        form_error($element, $valid);
      }
    }
  }
}

