<?php
   
    $quest_count = 0;

    function next_quest()
    {
        global $quest_count;
        $quest_count++;
        return $quest_count;
    }
        
    function create_form($title)
    {
        return "<div class=\"survey\"><table><tr><td valign=\"center\" colspan=\"2\">$title</td></tr>";    
    }

    function close_form()
    {
        return "</table></div>";
    }
    
    function create_scale($quest, $id)
    {
        $num = next_quest();
        $ans = "<div class=\"scale_question\"><tr><td width=\"5%\">$num</td><td>$quest</td></tr></div>";
        return $ans; 
    }
   
    function create_short_num($quest, $id)
    {
        $num = next_quest();
        $ans = "<div class=\"num_question\"><tr><td width=\"5%\">$num</td>
        <td>$quest</td></tr>
        </div>";
        return $ans;
    }
   
    function create_long($quest, $id)
    {
        $num = next_quest();
        $ans = "<div class=\"long_question\"><tr><td width=\"5%\">$num</td>
        <td>$quest</td></tr>
        </div>";
        return $ans;
    }

    function create_question($type, $quest, $id)
    {
        switch($type) {
            case 'scale':
                return create_scale($quest, $id);
                break;
            case 'short_num':
                return create_short_num($quest, $id);
                break;
            case 'long':
                return create_long($quest, $id);
                break;
    
            }
    }
    
?>
