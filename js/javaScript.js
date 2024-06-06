/*CLICKS INDEX*/
document.addEventListener("DOMContentLoaded", function() {
    var labels = document.querySelectorAll('label.contacts');
   
    // ir para outra página só a selecionar uma opção
    labels.forEach(function(label) {
        var link = label.querySelector('p');
        link.addEventListener('click', function() {
            window.location.href = "./html/contacts.html";
        });
    });

    //Localização da Notificação
    var infoElement = document.querySelector('p.info');
    
   
    if (infoElement) {
        var address = infoElement.textContent;
        updateLocation(address);
    }
});


// Função para atualizar a localização do iframe
function updateLocation(address) {
    var iframe = document.getElementById('gmap_canvas');
    iframe.src = `https://maps.google.com/maps?q=${encodeURIComponent(address)}&z=13&ie=UTF8&iwloc=&output=embed`;
}

