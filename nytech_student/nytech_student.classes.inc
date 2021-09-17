<?php

class NyTechStudentCreate {
  public $first;
  public $last;
  public $grade;
  public $dob;
  public $related_parent;
  public $entity;

  function __construct($first, $last, $grade, $dob, $related_parent) {
    $this->first = $first;
    $this->last = $last;
    $this->grade = (int) $grade;
    $this->dob = $dob;
    $this->related_parent = (int) $related_parent;
    $this->create();
  }

  private function create() {
    if(!empty($this->related_parent)) {
      $array = array(
        'type' => 'standard',
        'uid' => $this->related_parent,
        'first' => $this->first,
        'last' => $this->last,
        'grade' => $this->grade,
        'dob' => $this->dob,
        'binder_id' => $this->related_parent,
        'binder_type' => 'user',
        'created' => time(),
        'changed' => time(),
        'state' => 1,
        'uuid' => uuid_generate(),
      );

      $e = entity_create('student', $array);
      entity_save('student', $e);
      drupal_set_message('Student added.');
    }
  }
}

class NyTechStudentEdit {
  public $first;
  public $last;
  public $grade;
  public $dob;
  public $related_parent;
  public $entity;

  function __construct($e, $first, $last, $grade, $dob) {
    $this->entity = $e;
    $this->first = $first;
    $this->last = $last;
    $this->grade = (int) $grade;
    $this->dob = $dob;
    $this->save();
  }

  private function save() {
    if(is_object($this->entity)) {
      $this->entity->first = $this->first;
      $this->entity->last = $this->last;
      $this->entity->grade = $this->grade;
      $this->entity->dob = $this->dob;
      entity_save('student', $this->entity);
      drupal_set_message('Student updated.');
    }
  }
}


class NyTechUsersStudents {
  public $uid;
  public $ids;
  public $students;

  function __construct($uid = null) {
    $this->set_uid($uid);
    $this->set_students();
  }

  private function set_uid($uid = null) {
    if(empty($uid)) {
      global $user;
      $uid = $user->uid;
    }
    $this->uid = (int) $uid;
  }

  private function set_students() {
    if(!empty($this->uid)) {
      $ids = [];
      $students = [];
      $q = db_query('SELECT s.* FROM {student} s WHERE s.uid = :uid AND s.state = :state ORDER BY dob', array(':uid' => $this->uid, ':state' => 1));
      foreach($q as $row) {
        $ids[] = $row->id;
        $students[] = $row;
      }
      $this->ids = $ids;
      $this->students = $students;
    }
  }
}

class NyTechCalendar {
  public $days_labels = array();
  public $year = 0;
  public $month = 0;
  public $day = 0;
  public $date = 0;
  public $days_in_month = 0;
  public $running_day;
  public $week_count = 0;
  public $box_day_count;

  public $month_name;
  public $month_start_date;

  public $student_id;
  private $student;

  public $boxes;

  public function __construct($year = null, $month = null, $student_id = null) {
    $this->student_id = $student_id;
    $this->student = entity_load_single('student', $this->student_id);
    $this->set_days_labels();
    $this->set_dates($year, $month);
    $this->set_counts();
    $this->set_logs();
    $this->set_boxes();
    $this->set_output();
  }

  private function set_days_labels() {
    $this->days_labels = ["Sun", "Mon","Tue","Wed","Thu","Fri","Sat"];
  }

  private function set_dates($year = 0, $month = 0) {
    $this->year = (int) $year;
    $this->month = (int) $month;
    $this->date = date('Y-m-d');
    $this->month_start_date = (string) $this->year . '-' . (string) $this->month . '-1';
    $this->month_name = date('F', strtotime($this->month_start_date));
  }

  private function set_counts() {
    $this->running_day = (int) date('w',mktime(0,0,0, $this->month,1, $this->year));
    $this->days_in_month = (int) date('t',mktime(0,0,0, $this->month,1, $this->year));
    $this->week_count = (($this->days_in_month % 7 == 0 ? 0 : 1) + intval($this->days_in_month / 7));
    $this->box_day_count = (int) ($this->week_count * 7);

  }

  private function set_logs() {
    $logs = new NyTechStudentLogsManage($this->student, $this->year, $this->month, 1);
    $data = unserialize($logs->log->blob);
    $this->logs = $data['logs'];
  }

  private function get_log_info($year, $month, $day) {
    $link = $this->get_toggle($year, $month, $day, '+Add');
    $css_class = 'no_log';
    if(!empty($this->logs)) {
      if(!empty($this->logs[$year])) {
        if(!empty($this->logs[$year][$month])) {
          if(!empty($this->logs[$year][$month][$day])) {
            $link = $this->get_toggle($year, $month, $day, '( X )');
            $css_class = 'has_log';
          }
        }
      }
    }
    return array(
      'link' => $link,
      'class' => $css_class,
    );
  }

  private function set_boxes() {
    for($box_n = 1; $box_n <= $this->box_day_count; $box_n++) {
      if($box_n > $this->running_day && $box_n <= ($this->days_in_month + $this->running_day)) {
        $is_day = 'is_day_of_month';
        $day_n = ($box_n - $this->running_day);
        $this_day_date = (string) $this->year . '-' . (string) $this->month . '-' . (string) $day_n;
        $this_day_unix = strtotime($this_day_date);
        $toggle = $this->get_log_info($this->year, $this->month, $day_n);
        $has_log = $toggle['class'];
        $toggle_link = $toggle['link'];
      } else {
        $is_day = 'not_day_of_month';
        $day_n = '';
        $this_day_date = '';
        $this_day_unix = '';
        $has_log = '';
        $toggle_link = '';
      }

      $css_classes = array(
        'box--day',
        $is_day,
        $has_log,
      );

      $array[$box_n] = array(
        'box_n' => $box_n,
        'day_n' => $day_n,
        'unix' => $this_day_unix,
        'date' => $this_day_date,
        'css_classes' => implode(' ', $css_classes),
        'toggle' => $toggle_link,
      );

    }

    $this->boxes = $array;
  }

  private function set_output() {
    $output = '';

    if(!empty($this->boxes)) {
      foreach($this->boxes as $key => $box) {
        $new_line = '';
        if(($key%7) == 0) {
          $new_line = '<div class="clearfix"></div>';
        }
        $output .= theme('day_box', $box) . $new_line;
      }
    }

    $prev = $this->get_prev();
    $next = $this->get_next();

    $header = '<div class="pull-left">' . $prev . '</div>';
    $header .= '<div class="pull-right">' . $next . '</div>';
    $header .= '<h2>' . $this->month_name . ' ' . (string) $this->year . '</h2><div class="clearfix"></div>';
    $this->output = $header . '<div class="row"><div class="col-sm-12">' . $output . '</div></div>';
  }

  private function get_next() {
    $next_year = (string) $this->year;
    $next_month =  (string) ($this->month + 1);
    if($this->month == 12) {
      $next_month =  (string) 1;
      $next_year = (string) ($this->year + 1);
    }
    $student_id = (string) $this->student_id;
    $link = l('<i class="fa fa-arrow-circle-o-right"></i>', 'a/students/' . $student_id . '/days/' . $next_year . '/' . $next_month, array('html' => true, 'attributes' => array('class' => array('pull-right'))));
    return $link;
  }

  private function get_prev() {
    $prev_year = (string) $this->year;
    $prev_month =  (string) ($this->month - 1);
    if($this->month == 1) {
      $prev_month =  (string) 12;
      $prev_year = (string) ($this->year - 1);
    }
    $student_id = (string) $this->student_id;
    $link = l('<i class="fa fa-arrow-circle-o-left"></i>', 'a/students/' . $student_id . '/days/' . $prev_year . '/' . $prev_month, array('html' => true, 'attributes' => array('class' => array('pull-left'))));
    return $link;
  }

  private function get_toggle($year, $month, $day, $text = '+') {
    return l($text, 'api/v1/students/' . $this->student_id . '/days/' . $year . '/' . $month . '/' . $day, array('html' => true));
  }


}


class NyTechStudentLogsManage {
  public $student_id;

  public $year;
  public $month;
  public $day;

  public $data;

  public $log;
  private $student;


  function __construct($student, $year, $month, $day) {
    $this->student = $student;
    $this->student_id = $this->student->id;
    $this->year = $year;
    $this->month = $month;
    $this->day = $day;
    $this->set_logs();
  }

  private function set_logs() {
    if(!empty($this->student)) {
      $q = db_query('SELECT l.* FROM {instruction_record} l WHERE l.binder_id = :student_id', array(':student_id' => $this->student_id));
      $results = [];
      foreach ($q as $row) {
        $results[] = $row;
      }
      if(count($results) > 0) {
        $this->log = entity_load_single('instruction_record', $row->id);
      } else {
        $this->create_log();
      }
    }
  }

  private function create_log() {
    if(!empty($this->student_id)) {
      global $user;
      $log = entity_create('instruction_record', array(
        'type' => 'standard',
        'binder_type' => 'student',
        'binder_id' => $this->student_id,
        'uid' => $user->uid,
        'create' => time(),
        'changed' => time(),
        'state' => 1,
        'blob' => serialize([
          'binder_type' => 'student',
          'binder_id' => $this->student_id,
          'logs' => [],
        ]),
      ));
      entity_save('instruction_record', $log);
      $this->log = $log;
    }
  }

  public function toggle_day() {

    if(!empty($this->log)) {

      $this->blob_array = unserialize($this->log->blob);
      $log_this_day = true;
      if(!empty($this->blob_array['logs'][$this->year])) {
        if(!empty($this->blob_array['logs'][$this->year][$this->month])) {
          if(!empty($this->blob_array['logs'][$this->year][$this->month][$this->day])) {
            unset($this->blob_array['logs'][$this->year][$this->month][$this->day]);
            $log_this_day = false;
          }
        }
      }

      if($log_this_day == true) {
        $this->blob_array['logs'][$this->year][$this->month][$this->day] = 1;
      }
    }

    $this->log->blob = serialize($this->blob_array);
    entity_save('instruction_record', $this->log);
  }
}
