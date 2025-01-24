#include <ByteBoi.h>
#include <WiFi.h>
#include <CircuitOS.h>
#include <Loop/LoopManager.h>

// Display and Menu objects
Sprite *display;
List *wifiMenu;
TextElement *statusText;

// WiFi setup variables
const int MAX_NETWORKS = 10;
String networkNames[MAX_NETWORKS];
int networkCount = 0;
String selectedSSID = "";
String password = "";

// Player properties
struct Player {
  float x, y;
} player1, player2;

// Game state variables
enum State { MENU, CONNECTING, GAME };
State currentState = MENU;

// Game map
const int MAP_WIDTH = 8;
const int MAP_HEIGHT = 8;
const char gameMap[MAP_HEIGHT][MAP_WIDTH] = {
  {'#', '#', '#', '#', '#', '#', '#', '#'},
  {'#', ' ', ' ', ' ', ' ', ' ', ' ', '#'},
  {'#', ' ', '#', ' ', '#', ' ', ' ', '#'},
  {'#', ' ', '#', ' ', '#', '#', ' ', '#'},
  {'#', ' ', ' ', ' ', ' ', '#', ' ', '#'},
  {'#', '#', '#', ' ', '#', '#', ' ', '#'},
  {'#', ' ', ' ', ' ', ' ', ' ', ' ', '#'},
  {'#', '#', '#', '#', '#', '#', '#', '#'}
};

// Functions
void drawMap();
void connectToWiFi();

// Scan WiFi networks
void scanWiFi() {
  WiFi.mode(WIFI_STA);
  WiFi.disconnect();
  delay(100);
  networkCount = WiFi.scanNetworks();

  if (networkCount > MAX_NETWORKS) networkCount = MAX_NETWORKS;
  for (int i = 0; i < networkCount; i++) {
    networkNames[i] = WiFi.SSID(i);
  }
}

// WiFi menu setup
void setupWiFiMenu() {
  wifiMenu = new List();
  for (int i = 0; i < networkCount; i++) {
    wifiMenu->addItem(new ListItem(networkNames[i]));
  }

  wifiMenu->onSelect([](int index) {
    selectedSSID = networkNames[index];
    ByteBoi.getDisplay()->clear(TFT_BLACK);
    statusText = new TextElement("Enter password:");
    ByteBoi.getInput()->getKeyboard()->setBuffer(&password);
    ByteBoi.getInput()->getKeyboard()->setOnSubmit([]() {
      connectToWiFi();
    });
    statusText->draw();
    LoopManager::remove(wifiMenu);
  });
  LoopManager::add(wifiMenu);
}

// WiFi connection
void connectToWiFi() {
  ByteBoi.getDisplay()->clear(TFT_BLACK);
  display->drawString(10, 10, "Connecting to " + selectedSSID + "...");

  WiFi.begin(selectedSSID.c_str(), password.c_str());
  int retries = 0;
  while (WiFi.status() != WL_CONNECTED && retries < 10) {
    delay(500);
    retries++;
    display->fillRect(10, 20, 200, 20, TFT_BLACK);
    display->drawString(10, 20, "Retrying...");
  }

 

  if (WiFi.status() == WL_CONNECTED) {
    display->fillRect(10, 20, 200, 20, TFT_BLACK);
    display->drawString(10, 20, "Connected!");
    delay(1000);
    currentState = GAME;
  } else {
    display->fillRect(10, 20, 200, 20, TFT_BLACK);
    display->drawString(10, 20, "Failed to connect!");
    delay(1000);
    currentState = MENU;
    setupWiFiMenu();
  }
}

// Setup function
void setup() {
  ByteBoi.begin();
  display = ByteBoi.getDisplay();
  display->clear(TFT_BLACK);

  scanWiFi();
  setupWiFiMenu();
}

// Draw the game map
void drawMap() {
  for (int y = 0; y < MAP_HEIGHT; y++) {
    for (int x = 0; x < MAP_WIDTH; x++) {
      if (gameMap[y][x] == '#') {
        display->fillRect(x * 10, y * 10, 10, 10, TFT_WHITE); // Wall
      } else {
        display->fillRect(x * 10, y * 10, 10, 10, TFT_BLACK); // Floor
      }
    }
  }
}

// Game loop
void loop() {
  ByteBoi.update();

  if (currentState == MENU) {
    // Menu handled by LoopManager
  } else if (currentState == CONNECTING) {
    connectToWiFi();
  } else if (currentState == GAME) {
    // Draw map and players
    display->clear(TFT_BLACK);
    drawMap();
    display->fillCircle(player1.x * 10, player1.y * 10, 5, TFT_RED);  // Player 1
    display->fillCircle(player2.x * 10, player2.y * 10, 5, TFT_BLUE); // Player 2
    display->commit();
  }
}
