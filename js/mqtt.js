// mqtt.js

// Conectar ao broker MQTT (Mosquitto)
/*
const client = mqtt.connect('ws://test.mosquitto.org:1883'); // Substitua pelo endereço e porta do seu broker

client.on('connect', function () {
    console.log('Conectado ao broker MQTT (Mosquitto)');
    client.subscribe('bracelet/'); // Substitua pelo tópico que você quer assinar
});

client.on('message', function (topic, message) {
    const payload = message.toString();
    console.log(`Mensagem recebida: ${payload}`);

    // Verifique se a mensagem é 'crash'
    if (payload === 'crash') {
        Notificacao(); 
    }
});
*/
// Connect to the MQTT broker
const client = mqtt.connect('ws://test.mosquitto.org:1883'); // Replace with your broker's WebSocket URL

// Define the topic to subscribe to
const topic = 'bracelet/'; // Replace with your topic

// When the client is connected, subscribe to the topic
client.on('connect', () => {
    console.log('Connected to broker');
    client.subscribe(topic, (err) => {
        if (!err) {
            console.log(⁠ Subscribed to topic: ${topic} ⁠);
        }
    });
});

// Handle incoming messages
client.on('message', (topic, message) => {
    // Convert the message to a string and display it
    const msg = message.toString();
    console.log(⁠ Received message on ${topic}: ${msg} ⁠);
    
    // Append message to the messages div
    const messagesDiv = document.getElementById('messages');
    const newMessage = document.createElement('div');
    newMessage.textContent = ⁠ Topic: ${topic}, Message: ${msg} ⁠;
    messagesDiv.appendChild(newMessage);
});

function Notificacao() {
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
        info.textContent = "Bracelet Fall Detection"; // Adicione o conteúdo da notificação

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
