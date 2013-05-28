<?php
// $Id$

/**
 * 页面浏览
 * @param object $page
 */
function page_view($page) {
  dd_set_title(array($page->languages[$page->language]->title));
  
  module_invoke_all('page_view', $page);
   
  dd_add_css(dd_get_path('module', 'page') . '/page.css');
  return theme('page_view', $page);
}

/**
 * @Implement of hook_page_view()
 */
function page_page_view($page) {
  if ($page->type != 'form') return;

  $page->comment = new stdClass;
 
  if (page_form_is_access('view', $page)) {
    if ($data = comment_ext_view('page_form', $page->pid, 20, 1, 'desc')) {
      $page->comment = $data;
    }
  }
 
  if (page_form_is_access('add', $page)) {
    $comment = new stdClass;
    $comment->ext_type = 'page_form';
    $comment->ext_id = $page->pid;
    $comment->ext_id2 = 0;
    $comment->title = '';
    
    $page->comment->form = dd_get_form('comment_form', $comment);
  }
}

// public page_page_channel($type) {{{ 
/**
 * page_page_channel
 * 频道浏览页面
 * 
 * @param string $type 
 *  频道系统名称
 * @access public
 * @return string
 */
function page_page_channel($type) {
  if (!$info = page_get_channel_info($type)) dd_get_not();
  $channel = (object) $info;
  $channel->type = $type;
  $channel->content = '';

  if ($data = dd_get_storage('channel_' . $type)) {
  }

  return theme('page_channel', $channel);
}
// }}}
