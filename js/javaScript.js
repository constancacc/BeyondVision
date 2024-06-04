document.addEventListener("DOMContentLoaded", function() {
    var labels = document.querySelectorAll('label.contacts');
    var contact = document.querySelector('form');

    // ir para outra página só a selecionar uma opção
    labels.forEach(function(label) {
        var link = label.querySelector('p');
        link.addEventListener('click', function() {
            window.location.href = "contacts.html"; // Redireciona para a página desejada
        });
    });

    contact.submit();//submeter formulário sem botão
    
});