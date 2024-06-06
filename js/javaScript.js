let crash = "crash";


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
/*
if(crash == "crash"){
    let paragraph = document.querySelector('p#text_option');
    paragraph.style.display = "none";

    let container = document.querySelector('.noti2');

    let noti_container = document.createElement("div");
    noti_container.classList.add("noti_container");
    
    
    let date = document.createElement("p");
    date.classList.add("date");
    date.textContent = "7/06/2024"; // Adicione a data da notificação

    let info = document.createElement("p");
    info.classList.add("info");
    info.textContent = "Bracelet Fall Detection"; // Adicione o conteúdo da notificação

    let link = document.createElement("a");
    link.href = "./map.html";

    let button = document.createElement("img");
    button.classList.add("button");
    button.src = "../img/click.svg";
    

    link.appendChild(button);

    noti_container.appendChild(date);
    noti_container.appendChild(info);
    noti_container.appendChild(link);

    container.appendChild(noti_container);
}

console.log(crash);
*/