document.addEventListener("DOMContentLoaded", function() {
    var labels = document.querySelectorAll('label.contacts');

    // ir para outra página só a selecionar uma opção
    labels.forEach(function(label) {
        var link = label.querySelector('p');
        link.addEventListener('click', function() {
            window.location.href = "./contacts.html"; // Redireciona para a página desejada
        });
    });
    
});