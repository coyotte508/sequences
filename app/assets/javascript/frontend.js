$(function() {
   $("#delete").submit(function(e){
      e.preventDefault();
      if (confirm("Click OK to continue?")){
         this.submit();
      }
   });

   var lastStripped = null;

   $("#inputWords").on("input", function() {
    var stripped = $("#inputWords").text().toLowerCase().replace(/[^a-z]/g, '');
    $("#stripped").text(stripped);

    if (stripped === lastStripped) {
      return;
    }

    lastStripped = stripped;
    $.post("/decompose", {"sequence": stripped}, function(data) {
      console.log(arguments);
      $("#decomposed").text(data.replace(/\|/g, "-"));
    });
   });
});