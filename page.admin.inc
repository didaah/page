<?php
// $Id$

/**
 * @param string $type
 *  按类型查看静态页面列表
 */
function page_admin_lists($type) {
  $pages = module_invoke_all('page_info');

  if (empty($pages[$type])) dd_get_access();
  
  $info = $pages[$type];
  
  $tabs = array(
    l($info['title'], 'admin/page/' . $type),
    l('创建新页面', 'admin/page/' . $type . '/add')
  );
  
  dd_set_title(array($info['title']));
  
  if (isset($info['description']) && !arg(3)) {
    dd_set_help($info['description']);
  }
  
  switch (arg(3)) {
    case 'custom':
      if (!empty($_GET['pid']) && $page = page_load($_GET['pid'])) {
        // 自定义回调函数
        if (!empty($_GET['page_call']) && function_exists($_GET['page_call'])) {
          $output = call_user_func($_GET['page_call'], $page);
        }
      }
    break;

    case 'edit':
      if (!empty($_GET['pid']) && $page = page_load($_GET['pid'])) {
        if ($page->type != $type) {
          dd_get_access();
        }
        
        $tabs[] = l('编辑页面', $_GET['q']);
      }
    case 'add':
      
      if (!isset($page)) {
        $page = new stdClass;
        $page->type = $type;
      }
      
      $output = dd_get_form('page_form', $page, $info);
      
    break;
    
    case 'delete':
      if (!empty($_GET['pid']) && $page = page_load($_GET['pid'])) {
        if ($page->type != $type) {
          echo '非法操作';
        } else if (db_exec('DELETE FROM {pages} WHERE pid = ?', array($page->pid))) {
          module_invoke_all('page_delete', $page);
          echo 'tr';
        }
      }
      
    exit;

    case 'weight':
      if (!empty($_GET['pid']) && $page = page_load($_GET['pid'])) {
        if ($page->type != $type) {
          echo '非法操作';
        } else if (db_exec('UPDATE {pages} SET weight = ? WHERE pid = ?', array($_POST['value'], $page->pid))) {
          module_invoke_all('page_weight', $page);
          echo 'tr';
        }
      }
      
    exit;
    
    default:
      if ($fetch = pager_query('SELECT * FROM {pages} WHERE type = ? ORDER BY pid DESC',
      10, 0, NULL, array($type))) {

        global $conf;

        $table = array();

        foreach ($fetch as $o) {
          $info = '';
          if ($data = unserialize($o->data)) {
            $titles = array();
            foreach ($data['titles'] as $code => $title) {
              $titles[] = dd_get_strong($conf['languages'][$code]) . l($title, 'page/' . $type . '/' . $o->pid . '/' . $code, array('attributes' => array('target' => '_blank')));
            }
            $info = theme('item_list', $titles);
          }

          $links = array(
            'edit' => l('编辑', $_GET['q'] . '/edit', array('query' => 'pid=' . $o->pid)),
            'delete' => l('删除', $_GET['q'] . '/delete', array('query' => 'pid=' . $o->pid, 'attributes' => array('class' => 'confirmajax')))
          );

          module_alter_all('page_admin_links', $o, $links, $_GET['q']);

          $table[] = array(
            array('data' => $info, 'class' => 'left'),
            format_date($o->created),
            array(
              'data' => dd_form_weight(
                array(
                  '#value' => $o->weight,
                  '#start' => 0,
                  '#end' => 100,
                  '#attributes' => array(
                    'class' => 'changeconfirm dd_form_ajax_field',
                    'href' => url($_GET['q'] . '/weight', array('query' => 'pid='.$o->pid))
                  ),
                  '#name' => 'page_form_weight_' . $o->pid,
                )
              ),
            ),           
            dd_imp($links)
          );
        }
      
        $output = theme('table', array(t('page', '标题'), t('page', '时间'), t('page', '排序'), t('page', '操作')), $table, array('class' => 'center_table'));
        $output .= pager_view();
    
      } else {
        $output = system_no_content();
      }
  }
  
  dd_set_tabs($tabs);
  
  return $output;
}

// public page_admin_form_fields($page) {{{ 
/**
 * page_admin_form_fields
 * 自定义表单字段管理
 * 
 * @param object $page 
 * @access public
 * @return string
 */
function page_admin_form_fields($page) {
  if ($page->type != 'form') dd_get_access();

  dd_set_tabs(array(
    l(t('page', '自定义表单'), 'admin/page/form'),
    l(t('page', '创建新表单'), 'admin/page/form/add'),
    l(t('page', '!title字段管理', array('!title' => $page->data['titles'][$page->default_lang])), 'admin/page/form/fields/' . $page->pid),
  ));

  dd_set_sub_tabs(array(
    l(t('page', '所有字段'), 'admin/page/form/fields/' . $page->pid),
    l(t('page', '添加字段'), 'admin/page/form/fields/' . $page->pid . '/add'),
    l(t('page', '导出字段'), 'admin/page/form/fields/' . $page->pid . '/export'),
  ));

  switch (arg(5)) {
  	case 'add':
      $field->type = 'page_form_' . $page->pid;
      $field->module = 'page';
      $field->redirect = url('admin/page/form/fields/' . $page->pid);
  		$output = dd_get_form('field_form', $field);
  	break;
  	case 'export':
  	  $output = field_export(0, 'page', 'page_form_' . $page->pid);
  	break;
  	default:
  		$output = field_lists('page', 'page_form_' . $page->pid);
  }
 
  return $output; 
}
// }}}

/**
 * @Implement of hook_page_admin_links()
 */
function page_page_admin_links($page, &$links, $path) {
  if ($page->type == 'form') {
    $links['field'] = l(t('page', '表单字段'), 'admin/page/form/fields/' . $page->pid);
    $links['view'] = l(t('page', '查看留言'), 'page/form/' . $page->pid, array('fragment' => 'comment_view_wrapper'));
  }
}

/**
 * 频道内容定制
 * @param (string) $name
 *  频道 key
 */
function page_admin_channel($key) {

  if (!$info = page_get_channel_info($key)) dd_get_not();
  
  dd_set_breadcrumb(array(t('page', '!title内容维护', array('!title' => $info['title']))));
  
  if (empty($info['blocks']) && !is_array($info['blocks'])) {
    return system_no_content(t('page', '未定义任何区块，无法进行内容维护'));
  }
  
  $args = arg();
  
  if ($args[4]) {
    if (empty($info['blocks'][$args[4]]) || (!empty($args[5]) 
    && empty($info['blocks'][$args[4]]['block'][$args[5]]))) {
      dd_set_message(t('page', '错误的请求'));
      dd_goto('admin/page/channel/' . $key);
    }
    if (!empty($args[5])) {
      $right = dd_storage_admin('channel_' . $key, $args[4] . '_' . $args[5]);
    } else {
      $right = dd_storage_admin('channel_' . $key, $args[4]);
    }
  } else {
    $right = system_no_content(t('page', '请选择一个区块'));
  }
  
  $items = array();
  
  foreach ($info['blocks'] as $name => $block) {
    if (empty($block['title'])) continue;
    
    if (empty($block['group'])) {
      $items[] = l($block['title'], "admin/page/channel/$key/$name");
    } else if (!empty($block['block']) && is_array($block['block'])) {
      $item = array();
      foreach ($block['block'] as $n => $b) {
        if (empty($b['title'])) continue;
        $item[] = l($b['title'], "admin/page/channel/$key/$name/$n");
      }
      $items[] = theme('item_list', $item, $block['title'], 'ul', array('class' => 'admin_page_channel_block_links_sub'));
    }
    
  }
  
  $output = '<table id="admin_page_channel"><tr><td id="admin_page_channel_left" width="160px" valign="top">';
  $output .= theme('item_list', $items, NULL, 'ul', array('id' => 'admin_page_channel_block_links'));
  $output .= '</td><td id="admin_page_channel_right" valign="top">';
  $output .= $right;
  $output .= '</td></tr></table>';
  
  dd_add_css(dd_get_path('module', 'page') . '/page.admin.css');
  
  return $output;
}


