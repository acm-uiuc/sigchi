<?php
    include "create_questions.php";
    $title = 'SIGCHI Survey'; 
    include "heading.php";
    
    function print_survey($file)
    {
        $lines = file($file);
        foreach ($lines as $line_num => $line) {
            list($type, $id, $quest) = split("\t", $line);
            print create_question($type, $quest, $id);
        }
    }
?>
<?php
    print create_form('survey');
   
    print_survey('questions.txt');

    print close_form();
?>

<?php
include('footer.php');
?>
