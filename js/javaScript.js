document.addEventListener("DOMContentLoaded", function() {
    var labels = document.querySelectorAll('label.contacts');
   

    // ir para outra página só a selecionar uma opção
    labels.forEach(function(label) {
        var link = label.querySelector('p');
        link.addEventListener('click', function() {
            window.location.href = "./html/contacts.html"; // Redireciona para a página desejada
        });
    });
});

function initMap() {
    // Tenta obter a localização atual do usuário
    if (navigator.geolocation) {
        navigator.geolocation.getCurrentPosition(function(position) {
            var lat = position.coords.latitude;
            var lng = position.coords.longitude;
            var iframe = document.getElementById('gmap_canvas');
            iframe.src = `https://maps.google.com/maps?q=${lat},${lng}&z=13&ie=UTF8&iwloc=&output=embed`;
        }, function() {
            handleLocationError(true);
        });
    } else {
        // Browser não suporta Geolocation
        handleLocationError(false);
    }
}

function handleLocationError(browserHasGeolocation) {
    console.log(browserHasGeolocation ?
        'Erro: O serviço de Geolocalização falhou.' :
        'Erro: Seu navegador não suporta geolocalização.');
}

// Função para mudar a localização do mapa
function changeLocation(address) {
    var location_title = document.querySelector('h4.location_title');
    location_title.style.display = "none";
    
    var iframe = document.getElementById('gmap_canvas');
    iframe.src = `https://maps.google.com/maps?q=${encodeURIComponent(address)}&z=13&ie=UTF8&iwloc=&output=embed`;
}

// Inicializa o mapa assim que a página é carregada
window.onload = initMap;
