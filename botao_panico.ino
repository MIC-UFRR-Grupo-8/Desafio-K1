#include <WiFi.h>                                                     // Biblioteca para conectar o ESP32 ao WiFi
#include <HTTPClient.h>                                               // Biblioteca para fazer requisições HTTP //
#include <UrlEncode.h>                                                // Biblioteca para codificar texto em formato de URL, disponível na lista de bibliotecas do Arduino : "UrlEncode" de Masayuki Sugahara  //

// Definição das credenciais do WiFi
const char* ssid = "REDEWIFI";             
char* password = "PASSWIFI";          

// Definição dos pinos do botão e LEDs
#define botao 21  // Pino de conexão no ESP para o botão
#define led1 23   // Pino conexão no ESP para o LED externo PROTOBOARD (acende ao enviar mensagem)
#define led2 2    // Pino para o LED inteno do ESP (indica o status de conexão)

bool estadoLed = false; // Variável para controlar o estado do LED piscante
bool mensagemEnviada = false; // Variável para evitar múltiplos envios

// Número de telefone e chave da API para CallMeBot
String phoneNumber1 = "559599999999";      // Número de telefone para enviar mensagem (Brasil 55) Exemplo : 559599999999 
String apiKey1 = "9999999";                // Chave de API para autenticação no CallMeBot, é dada pelo whats CallMeBot após enviar a mensagem : "I allow callmebot to send me messages" 
String phoneNumber2 = "559599999999";       
String apiKey2 = "9999999";

// Função para enviar a mensagem via CallMeBot API
void sendMessage(String phoneNumber, String apiKey, String message) {
  String url = "http://api.callmebot.com/whatsapp.php?phone=" + phoneNumber + "&apikey=" + apiKey + "&text=" + urlEncode(message);
  HTTPClient http;  // Cria um objeto HTTPClient
  http.begin(url);  
 
  // Adiciona o cabeçalho indicando que a requisição está no formato x-www-form-urlencoded
  http.addHeader("Content-Type", "application/x-www-form-urlencoded");
  
  // Envia a requisição POST e armazena o código de resposta HTTP
  int httpResponseCode = http.POST(url);
  if (httpResponseCode == 200) {  // Código 200 indica sucesso
    Serial.println("Mensagem enviada com sucesso");
  } else {  // Qualquer outro código indica erro
    Serial.println("Erro no envio da mensagem");
    Serial.print("HTTP response code: ");
    Serial.println(httpResponseCode);
  }

  http.end();  // Libera e fecha os recursos usados pela requisição HTTP
}

void setup() {
  Serial.begin(115200);          
  pinMode(botao, INPUT_PULLUP);  // Configura o pino do botão como entrada com pull-up interno(retorna 0 ou 1 de acordo com o valor de tensão lido)
  pinMode(led1, OUTPUT);         // Configura o pino do LED1 como saída
  pinMode(led2, OUTPUT);         // Configura o pino do LED2 como saída

  digitalWrite(led1, LOW);       // Inicialmente, os LEDs ficam apagados
  digitalWrite(led2, LOW);

  WiFi.begin(ssid, password);    
  Serial.println("Conectando");

  // Enquanto não estiver conectado ao WiFi, pisca o LED2
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);                              // Aguarda 500 ms
    Serial.print(".");                       // Imprime um ponto para indicar tentativa de conexão
    digitalWrite(led2, !digitalRead(led2));  // Inverte o estado do LED2 (pisca)
  }

  // Quando conectado ao WiFi
  Serial.println("");
  Serial.print("Conectado ao WiFi neste IP ");
  digitalWrite(led2, HIGH);        // Mantém o LED2 aceso indicando conexão bem-sucedida
  Serial.println(WiFi.localIP());  
}

void loop() {

  static bool botaoAnterior = HIGH; // Estado anterior do botão
  static unsigned long ultimaTrocaLed = 0; // Armazena o tempo para controle do piscar do LED
  const unsigned long intervaloPiscar = 500; // Intervalo de tempo para o LED piscar

  bool estadoBotao = digitalRead(botao); 
  
  // Detecta a mudança de estado do botão
  if (estadoBotao == LOW && botaoAnterior == HIGH) {
    estadoLed = !estadoLed; // Alterna o estado do LED piscante
    if (estadoLed && !mensagemEnviada) {
      sendMessage(phoneNumber1, apiKey1, "SOCORRO!!!!, Help me");
      sendMessage(phoneNumber2, apiKey2, "SOCORRO!!!!, Help me");
      mensagemEnviada = true;
    } else if (!estadoLed) {
      mensagemEnviada = false; // Permite o envio de nova mensagem na próxima ativação
    }
  }
  botaoAnterior = estadoBotao; // Atualiza o estado anterior do botão

  // Faz o LED1 piscar se o estado for ativo
  if (estadoLed) {
    if (millis() - ultimaTrocaLed >= intervaloPiscar) {
      ultimaTrocaLed = millis();
      digitalWrite(led1, !digitalRead(led1)); // Inverte o estado do LED1
    }
  } else {
    digitalWrite(led1, LOW); // Mantém o LED apagado se o estado for inativo
  }

}
