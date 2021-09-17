<?php

module_load_include('inc', 'nytech_student', 'nytech_student.classes');


function nytech_student_form($form, &$form_state, $id = null) {

  $format = 'Y-m-d';
  $grades = array(
    '0' => 'k',
    '1' => 1,
    '2' => 2,
    '3' => 3,
    '4' => 4,
    '5' => 5,
    '6' => 6,
    '7' => 7,
    '8' => 8,
    '9' => 9,
    '10' => 10,
    '11' => 11,
    '12' => 12,
  );

  $first = '';
  $last  = '';
  $grade = 0;
  $dob = date('Y-m-d');
  $action = 'create';

  if(!empty($id)) {
    $e = entity_load_single('student', $id);
    if(is_object($e)) {
      $first  = $e->first;
      $last   = $e->last;
      $grade  = $e->grade;
      $dob    = date('Y-m-d', $e->dob);
      $id     = $e->id;
      $action = 'update';
    }
    $archive = '
      <div class="pull-right">'
      . l('Archive Student', 'a/students/' . $id . '/archive', array('attributes' => array('class' => array('btn', 'btn-danger')))) .
      '</div>'
    ;
  } else {
    $id = '';
    $archive = '';
  }

  $form['start'] = array(
    '#markup' => '<div class="col-sm-12 col-md-8 col-lg-8 col-md-offset-2 col-lg-offset-2"><div class="well"><h4>Add student</h4>',
  );

  $form['first'] = array(
    '#title' => 'First name',
    '#type' => 'textfield',
    '#required' => true,
    '#default_value' => $first,
    '#attributes' => array(
      'placeholder' => 'John',
    ),
  );

  $form['last'] = array(
    '#title' => 'Last name',
    '#type' => 'textfield',
    '#required' => true,
    '#default_value' => $last,
    '#attributes' => array(
      'placeholder' => 'Smith',
    ),
  );

  $form['grade'] = array(
    '#title' => 'Grade',
    '#type' => 'select',
    '#default_value' => $grade,
    '#options' => $grades,
    '#required' => true,
  );

  $form['dob'] = array(
    '#title' => 'Birthday',
    '#type' => 'date_select',
    '#date_format' => $format,
    '#default_value' => $dob,
    '#date_label_position' => 'within',
    '#date_year_range' => '-20:1',
  );

  $form['id'] = array(
    '#type' => 'hidden',
    '#value' => $id,
  );

  $form['action'] = array(
    '#type' => 'hidden',
    '#value' => $action,
  );

  $form['submit'] = array(
    '#type' => 'submit',
    '#value' => 'Save',
    '#prefix' => '<br /><br />',
  );

  $form['archive'] = array(
    '#markup' => $archive,
  );

  $form['end'] = array(
    '#markup' => '</div></div>',
  );

  $form['#validate'][] = 'nytech_student_form_validate';
  $form['#submit'][]   = 'nytech_student_form_submit';

  return $form;
}

function nytech_student_form_validate($form, &$form_state) {
  $values = $form_state['values'];
}

function nytech_student_form_submit($form, &$form_state) {
  global $user;
  $values = $form_state['values'];
  $dob = strtotime($values['dob']);
  if($values['action'] == 'update') {
    $e = entity_load_single('student', $values['id']);
    new NyTechStudentEdit($e, $values['first'], $values['last'], $values['grade'], $dob);
  }
  if($values['action'] == 'create') {
    new NyTechStudentCreate($values['first'], $values['last'], $values['grade'], $dob, $user->uid);
  }
}
