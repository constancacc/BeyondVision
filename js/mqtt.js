// mqtt.js

// Conectar ao broker MQTT (Mosquitto)
const client = mqtt.connect('ws://YOUR_MOSQUITTO_BROKER_ADDRESS:YOUR_PORT'); // Substitua pelo endereço e porta do seu broker

client.on('connect', function () {
    console.log('Conectado ao broker MQTT (Mosquitto)');
    client.subscribe('your/mqtt/topic'); // Substitua pelo tópico que você quer assinar
});

client.on('message', function (topic, message) {
    const payload = message.toString();
    console.log(`Mensagem recebida: ${payload}`);

    // Verifique se a mensagem é 'crash'
    if (payload === 'crash') {
        Notificação();
        
    }
});

function Notificação() {
    let paragraph = document.querySelector('p#text_option');
    paragraph.style.display = "none";
    
    let container = document.querySelector('.noti'); // Seleciona o container correto

   
        // Cria um novo container de notificação
        let noti_container = document.createElement("div");
        noti_container.classList.add("noti_container");
        
        let date = document.createElement("p");
        date.classList.add("date");
        date.textContent = "17/02/2024"; // Adicione a data da notificação

        let info = document.createElement("p");
        info.classList.add("info");
        info.textContent = "New Notification Content"; // Adicione o conteúdo da notificação

        let link = document.createElement("a");
        link.href = "./map.html";

        let button = document.createElement("img");
        button.src = "../img/click.svg";
        button.classList.add("button");

        link.appendChild(button);

        noti_container.appendChild(date);
        noti_container.appendChild(info);
        noti_container.appendChild(link);

        container.appendChild(noti_container);
}
