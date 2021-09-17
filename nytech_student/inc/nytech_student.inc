<?php

module_load_include('inc', 'nytech_student', 'nytech_student.form');
module_load_include('inc', 'nytech_student', 'nytech_student.classes');

function endpoint_students() {
  $data = array();
  $result = db_query('SELECT * FROM {student}', array());
  foreach($result as $r) {
    $data[] = $r;
  }
  return $data;
}

function screen_students() {
  global $user;
  $form_id = 'nytech_student_form';
  $form = drupal_get_form($form_id);
  $s = new NyTechUsersStudents();
  $output = '';
  foreach($s->students as $student) {
    $output .= '<div class="well">';
    $output .= l($student->first . ' ' . $student->last . '(Grade ' . $student->grade . ') '  . date('m/d/Y', $student->dob), 'a/students/' . $student->id) . '<br />';
    $output .= '</div>';
  }
  return $output . drupal_render($form);
}

function screen_student_edit($id) {
  $form_id = 'nytech_student_form';
  $form = drupal_get_form($form_id, $id);
  $report_link = l('Report', 'a/students/' . $id . '/days/report', array('attributes' => array('class' => array('btn', 'btn-sm', 'btn-primary'))));
  $log_link = l('Log Days', 'a/students/' . $id . '/days/' . date('Y') . '/' . date('m'), array('attributes' => array('class' => array('btn', 'btn-sm', 'btn-primary'))));
  return $log_link . ' ' . $report_link . '<br /><hr /><h4>Edit Student</h4>' . drupal_render($form);
}

function screen_student_archive($id) {
  $e = entity_load_single('student', $id);
  $e->state = 0;
  entity_save('student', $e);
  drupal_set_message('Student archived.');
  drupal_goto('a/students');
}

function screen_student_logs($id) {
  $year = date('Y');
  $month = date('m');
  $calendar = new NyTechCalendar($year, $month);
  return $calendar->output;
}

function screen_student_logs_select($id, $year, $month)  {
  $calendar = new NyTechCalendar($year, $month, $id);
  return $calendar->output;
}

function screen_student_logs_toggle($id, $year, $month, $day) {
  $student = entity_load_single('student', $id);
  $logs = new NyTechStudentLogsManage($student, $year, $month, $day);
  $logs->toggle_day();
  drupal_set_message('Log updated.');
  drupal_goto('a/students/' . $id . '/days/' . $year . '/' . $month);
}

function screen_student_logs_report($id) {
  $output = '';
  $student = entity_load_single('student', $id);
  $record = new NyTechStudentLogsManage($student, date('Y'), date('m'), date('d'));
  $instruction_days = [];
  $total = [];
  $data = unserialize($record->log->blob);

  $required = 180;

  if(!empty($data['logs'])) {
    foreach($data['logs'] as $year_key => $year) {
      $year_name = date('Y', strtotime($year_key . '-1-1'));
      foreach($year as $month_key => $month) {
        $month_name = date('F', strtotime($year_key . '-' . $month_key . '-1'));
        $instruction_days[$year_name . ' ' . $month_name] = count($month);
        foreach($month as $day_key => $day) {
          $total[] = $day;
        }
      }
    }

    $total = count($total);
    foreach($instruction_days as $key => $value) {
      $output .= '<h4>' . $key . ': ' . $value . ' days</h4>';
    }

    $balance = $required - $total;
    $output .= '<h4>STATS</h4>Needed: ' . $required . '<br />Days Remaining' . $balance;

  }

  return '<h2>' . $total . ' days for ' . $student->first . ' ' . $student->last .  '</h2>' . $output;
}
