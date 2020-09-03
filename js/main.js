$(window).load(function() {

    $("#modal-background").click(function() {
        $("#modal").css("opacity", "0");
        $("#modal-background").css("opacity", "0");
        setTimeout(function() {
            $("#modal").css("z-index", "-1");
            $("#modal-background").css("z-index", "-1");
        }, 200);
    });
   
});


function openModal() {
	$("#modal").css("z-index", "11");
    $("#modal-background").css("z-index", "10");
	$("#modal").css("opacity", "1");
	$("#modal-background").css("opacity", "0.6");
}