<?php
// $Id$

/**
 * 频道数据区块模板
 * @param object $channel 频道对象，$channel->type 为频道系统名称
 * @param object $block 区块对象，$block->type 为区块数据类型，$block->delta 为区块系统 id
 * 模板加载顺序：
 *  channel_block_page_{$block->type}_{$channel->type}_{$block->delta}.tpl.php
 *  channel_block_page_{$block->type}_{$channel->type}.tpl.php
 *  channel_block_page_{$block->type}.tpl.php
 *  channel_block_page.tpl.php
 */
?>

<div class="channel_block_page channel_block_page_<?php echo $channel->type;?> channel_block_page_<?php echo $channel->type;?>_<?php echo $block->type;?> channel_block_page_<?php echo $channel->type;?>_<?php echo $block->type;?>_<?php echo $block->delta?>">
  <?php echo $block->content;?>
</div>
