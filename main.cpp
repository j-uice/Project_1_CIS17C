/*
Blackjack STL Project
Julia Therese Roldan

This is a C++ project implementing Blackjack using the STL library
(used chatgpt for a template and modified it to fit the requirements of the project)
*/

#include <bits/stdc++.h>

using namespace std;

/* --- Card Structure --- */
struct Card 
{
    string suit; // Hearts, Diamonds, Clubs, Spades
    string rank; // 2-10, J, Q, K, A
    int value;   // Numeric value of the card for scoring

    Card(string s, string r, int v) : suit(s), rank(r), value(v) {}
}; 
/* --- End of Card Structure --- */

/* --- Player Class --- */
class Player 
{
public:
    string name;
    // STL CHECKOFF: list
    list<Card> hand; // Player's hand of cards
    int balance;     // Player's balance
    int bet;         // Current bet
    int wins = 0;
    bool eldritchMode = false;

    // Default constructor
    Player() : name(""), balance(0), bet(0) {}

    // Parameterized constructor
    Player(string n, int b) : name(n), balance(b), bet(0) {}

    // Function to calculate the total value of the player's hand
    int handValue() const 
    {
        int total = 0; // Total value of the hand
        int aces = 0;  // Count Aces separately

        // Loop through the cards in the hand
        for (const auto& card : hand) 
        {
            total += card.value;
            if (card.rank == "A") aces++;
        }

        // Adjust for Aces
        while (total > 21 && aces > 0) 
        {
            // Count Ace as 1 instead of 11
            total -= 10;
            aces--;
        }

        return total;
    }

    void clearHand() 
    {
        // Clear the player's hand
        hand.clear();
    }

    void addCard(const Card& card) 
    {
        // Add a card to the player's hand
        hand.push_back(card);
    }
}; 
/* --- End of Player Class --- */

// Function prototypes
// STL CHECKOFF: forward_list
void initializeDeck(forward_list<Card>& deck);
void shuffleDeck(forward_list<Card>& deck);
void dealCard(forward_list<Card>& deck, Player& player);
void showHand(const Player& player, bool eldritchMode);
void placeBet(Player& player, bool eldritchMode);
void playRound(forward_list<Card>& deck, Player& player, Player& dealer, bool& eldritchMode);
void settleBets(Player& player, Player& dealer, bool eldritchMode);
int countFaceCards(const Card* hand, int size);
bool isWinningHand(const Player& player, int dealerValue);
list <Player> extractWinners(const list<Player>& players, int dealerValue);
bool isEldritchTriggered(const Player& player);

// Constants
const int MAX_NAME_LENGTH = 8;

/* --- Main Function --- */
int main () 
{
    string playerName;
    bool eldritchMode = false;  // eldritch flag

    cout << "Welcome To Our Blackjack Table!" << endl;

    // Name input and check
    do 
    {
        cout << "Your name dear player? (max " << MAX_NAME_LENGTH << " characters): ";
        getline(cin, playerName);

        // Name length check
        if(playerName.length() > MAX_NAME_LENGTH) 
        {
            cout << "That name is too long! Try something shorter will ya?\n";
        }

    } while (playerName.length() > MAX_NAME_LENGTH);

    // Initialize player with a balance of 1000
    Player player(playerName, 1000); 

    // Initialize the deck of cards
    forward_list<Card> deck;    // Deck of cards
    initializeDeck(deck);       // Initialize the deck of cards
    Player dealer("Dealer", 0); // Dealer player with no balance

    char cont = 'y'; // Variable to control the game loop

    // Game loop to play multiple rounds
    while (cont == 'y') 
    {
        shuffleDeck(deck);                             // Shuffle the deck before each round
        playRound(deck, player, dealer, eldritchMode); // Play a round of Blackjack
        
        // Check if eldritch mode should be triggered after round
        if (!eldritchMode && isEldritchTriggered(player)) 
        {
            eldritchMode = true;
            cout << "\nSomething shifts in the air... The cards whisper secrets only you can hear...\n";
        }

        if (player.balance <= 0) 
        {
            cout << (eldritchMode
                ? "\nYour balance is dust, mortal. Your soul shall be theirs and your hostless husk shall roam this world. \nThe Ancient Ones thank your addiction.\n"
                : "You're broke?! Already?! No more bets, no more games. Come back richer!\n");
                exit(0);
        }
        
        string input;
        do {
            cout << (eldritchMode 
                ? "\nThe Ancient Ones give you mercy. Will you defy their mercy or choose the wiser? (y/n): " 
                : "\n...Anyways! Would you like to play another round my dear player? (y/n): ");

            getline(cin, input);

            transform(input.begin(), input.end(), input.begin(), ::tolower);

        } while (input != "y" && input != "n");

        cont = input[0];

    }

    // Display final balance of player
    cout << "\n---Final Earnings---" << endl;
    cout << player.name << ": $" << player.balance << endl;

    cout << (eldritchMode 
        ? "The Ancient Ones will miss you but may you never find us again.\n"
        : "I'll miss you my little addict. Play again soon :)\n");

    cout << endl;


    return 0; // End of the program
}
/* --- End of Main Function --- */

/* --- Initilize Deck Function --- */
void initializeDeck(forward_list<Card>& deck) 
{
    deck.clear(); // Clear the deck if it already exists
    string suits[] = {"Hearts", "Diamonds", "Clubs", "Spades"};
    string ranks[] = {"2", "3", "4", "5", "6", "7", "8", "9", "10", "J", "Q", "K", "A"};
    int values[] = {2, 3, 4, 5, 6, 7, 8, 9, 10, 10, 10, 10, 11}; // Values for cards

    // Loop through suits and ranks to create a full deck of cards
    for (const auto& suit : suits) 
    {
        for (int i = 0; i < 13; ++i) 
        {
            // Add cards to the deck
            deck.push_front(Card(suit, ranks[i], values[i]));
        }
    }
}
/* --- End of Initilize Deck Function --- */

/* --- Shuffle Deck Function --- */
void shuffleDeck(forward_list<Card>& deck) 
{
    // STL CHECKOFF: list
    list<Card> tempDeck(deck.begin(), deck.end()); // Convert forward_list to list for shuffling

    // Random number generator
    unsigned seed = chrono::system_clock::now().time_since_epoch().count();
    mt19937 gen(seed);

    // Shuffle manually by repeatedly removing random elements
    list<Card> shuffledDeck;

    // Loop until the temporary deck is empty
    while (!tempDeck.empty()) 
    {
        uniform_int_distribution<> dist(0, tempDeck.size() - 1); // Distribution for random index
        int index = dist(gen);                                   // Generate a random index

        auto it = tempDeck.begin();
        advance(it, index);                                      // Move iterator to the random index

        shuffledDeck.push_back(*it);                             // Add the card at the random index to the shuffled deck
        tempDeck.erase(it);                                      // Remove the card from the temporary deck
    }

    // STL CHECKOFF: queue
    queue<Card> deckQueue;
    for (auto& card : shuffledDeck)
        deckQueue.push(card);

    deck.clear();
    while (!deckQueue.empty()) 
    {
        deck.push_front(deckQueue.front());
        deckQueue.pop();
    }

}
/* --- End of Shuffle Deck Function --- */

/* --- Deal Card Function --- */
void dealCard(forward_list<Card>& deck, Player& player) 
{
    // Statement to check if the deck is not empty before dealing a card
    if (!deck.empty()) 
    {
        player.addCard(deck.front()); // Add the top card from the deck to the player's hand
        deck.pop_front();             // Remove the card from the deck
    }
}
/* --- End of Deal Card Function --- */

/* --- Show Hand Function --- */
// showHand function ends here
void showHand(const Player& player, bool eldritchMode) 
{
    cout << "------------------------" << endl;
    cout << player.name << "'s hand: ";

    list<Card> sortedHand(player.hand.begin(), player.hand.end());

    // STL CHECKOFF: sort
    sortedHand.sort([](const Card& a, const Card& b) 
    {
        return a.value < b.value;
    });

    // STL CHECKOFF: map
    map<string, int> rankCount;

    // STL CHECKOFF: set
    set<string> seenSuits;

    // Use for_each to fill the set (non-mutating algorithm)
    for_each(player.hand.begin(), player.hand.end(), [&](const Card& card) 
    {
        seenSuits.insert(card.suit);
    });

    // Print the sorted hand only once
    for (auto& card : sortedHand) 
    {
        string displaySuit = card.suit;

        if (eldritchMode) 
        {
            // STL CHECKOFF: replace
            replace(displaySuit.begin(), displaySuit.end(), 'a', '@');
        }

        cout << "[" << card.rank << " of " << displaySuit << "] ";
        rankCount[card.rank]++;
    }

    cout << " (Value: " << player.handValue() << ")\n";

    cout << "You hold: ";
    for (const auto& pair : rankCount) 
    {
        cout << pair.second << "x " << pair.first << " ";
    }
    cout << endl;
    cout << "------------------------" << endl;
}
/* --- End of Show Hand Function --- */

/* --- Place Bet Function --- */
void placeBet(Player& player, bool eldritchMode) 
{
    while (true) 
    {
        cout << (eldritchMode
            ? "\nPlace your tribute dear mortal (balance: "
            : "\nMy dear " + player.name + ", enter your bet (balance: ")
            << player.balance << "): ";

        cin >> player.bet;
        cin.ignore(numeric_limits<streamsize>::max(), '\n');

        // Check for non-numeric input
        if (cin.fail()) 
        {
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');

            cout << (eldritchMode
                ? "\nDoes one know the difference between numbers and madness, mortal?"
                : "\nDo you know the difference between a number and letters dear my dear addict?")
                << "Try again.\n";

            continue;
        }

        if (player.bet < 0) 
        {
             cout << (eldritchMode
                ? "\nYou cannot un-give to the Ancient Ones. No refunds or cowardice allowed dear mortal."
                : "\nYou cannot be that broke... Could you? Try again and prove me wrong will ya?");

            continue;
        }

        if(player.bet > player.balance) 
        {
            cout << (eldritchMode
                ? "\nYou dare bet more than you possess?! Try again and offend the Ancient Ones no more."
                : "\nDo you need to get your eyes checked? Maybe instead of betting, you should use the money for an optometry visit.\nAlas, I want your money so try again.");

                continue;
        }

        // Check for betting 0
        if (player.bet == 0) 
        {
            cout << (eldritchMode
                ? "\nYou have more than enough to bet dear mortal. Scared under the gaze of the Ancient Ones? Prove us wrong."
                : "\nOh come on now dear player, I know you've got more money to give. Prove me wrong will ya?");

            continue;
        }

        break; // Bet is valid
    }
}
/* --- End Place Bet Function --- */

/* --- Play Round Function --- */
void playRound(forward_list<Card>& deck, Player& player, Player& dealer, bool& eldritchMode) 
{
    player.clearHand(); // Clear the player's hand at the start of the round
    dealer.clearHand(); // Clear the dealer's hand at the start of the round

    placeBet(player,eldritchMode);

    // Deal initial 2 cards to player and dealer
    for (int i = 0; i < 2; ++i) 
    {
        dealCard(deck, player);
        dealCard(deck, dealer);
    }

    // Check for natural blackjacks before player takes action
    bool playerBlackjack = (player.hand.size() == 2 && player.handValue() == 21);
    bool dealerBlackjack = (dealer.hand.size() == 2 && dealer.handValue() == 21);

    if (playerBlackjack || dealerBlackjack) 
    {
        showHand(player, eldritchMode);
        showHand(dealer, eldritchMode);

        if (playerBlackjack && dealerBlackjack) 
        {
            cout << (eldritchMode
                    ? "\nTwo perfect hands clash. The stars hold their breath... (Push)\n"
                    : "\nARE YOU KIDDING ME?!\n");

            player.balance += player.bet; // return the bet

        } else if (playerBlackjack) 
        {
            cout << (eldritchMode
                    ? "\nA divine hand! You are chosen... (Blackjack Win)\n"
                    : "\nA BLACKJACK?! IT CAN'T BE!\n");

            player.balance += player.bet * 2.5; // Pays 3:2
            player.wins++;

        } else 
        {
            cout << (eldritchMode
                    ? "\nThe dealer opens with darkness... Blackjack.\n"
                    : "\n Well well well. Blackjack my dear little addict. The Gods watch over me.\n");

            player.balance -= player.bet; // Dealer wins
        }

        // Discard hands to discard pile
        // STL CHECKOFF: stack
        static stack<Card> discardPile;
        for (auto& c : player.hand) discardPile.push(c);
        for (auto& c : dealer.hand) discardPile.push(c);

        return; // End round early
    }

    showHand(player, eldritchMode); // Show player's hand

    // Show dealer's first card only (hide second card)
    auto it = dealer.hand.begin();

    if (it != dealer.hand.end()) 
    {
        string dealerSuit = it->suit;

        if (eldritchMode) 
        {
            replace(dealerSuit.begin(), dealerSuit.end(), 'a', '@');
        }

        cout << "Dealer's hand: [" << it->rank << " of " << dealerSuit << "] [Hidden]\n";
    }

    cout << "------------------------" << endl;

    char action;

    // Loop to allow the player to hit or stand
    do {
        cout << (eldritchMode
                ? "My dear mortal, do you want to hit (h) or stand (s) "
                : "My dear, " + player.name + ", do you want to hit (h) or stand (s)? ");
        cin >> action; // Get the player's action
        cin.ignore(numeric_limits<streamsize>::max(), '\n');

        if (action == 'H' || action == 'h') 
        {
            dealCard(deck, player);         // Deal a card if the player chooses to hit
            showHand(player, eldritchMode); // Show the updated hand
        }

    } while ((action != 'S' && action != 's') && player.handValue() <= 21); // Continue until player stands or busts

    // Loop for the dealer's turn
    if (player.handValue() <= 21) 
    {
        cout << "Dealer's turn...\n";

        showHand(dealer, eldritchMode);

        while (dealer.handValue() < 17) 
        {
            dealCard(deck, dealer);
            showHand(dealer, eldritchMode);
        }
    }

    settleBets(player, dealer, eldritchMode); // Settle bets after all actions

    // A stack as a discard pile
    static stack<Card> discardPile;
    for (auto& c : player.hand) discardPile.push(c);
    for (auto& c : dealer.hand) discardPile.push(c);
}
/* --- End of Play Round Function --- */

/* --- Settle Bet Function --- */
void settleBets(Player& player, Player& dealer, bool eldritchMode) 
{
    int dealerValue = dealer.handValue(); // Get the dealer's hand value
    int playerValue = player.handValue();

    // In playRound, after valid bet is accepted
    player.balance -= player.bet; // Deduct the bet immediately

    if (playerValue > 21) 
    {
        cout << (eldritchMode 
            ? "\nThe void devours your folly... (BUSTED)\n" 
            : "\nBusted! Too bad so sad ):\n");

    } else if (dealerValue > 21 || playerValue > dealerValue) 
    {
        cout << (eldritchMode 
            ? "\nThe Ancient Ones smile upon your triumph...\n" 
            : "\nYOU WON?! HOW?! D:<\n");

        player.balance += player.bet * 2;
        player.wins++;

    } else if (playerValue == dealerValue) 
    {
        cout << (eldritchMode ? "\nBalance is a fleeting illusion in this cosmic game.\n" : "\nStop copying me!\n");

        player.balance += player.bet;
    } else 
    {
        cout << (eldritchMode 
            ? "\nThe shadows claim your stake...\n" 
            : "\nThe Gods are not in your favor! how unfortunate :(\n");
    }
}
/* --- End of Settle Bet Function --- */

/* --- Face Cards Count Function --- */
int countFaceCards(const Card* hand, int size)
{
    int count = 0;
    const Card* it = hand;           // trivial iterator (pointer)
    const Card* end = hand + size;   // end pointer

    while (it != end)
    {
        if (it->rank == "J" || it->rank == "Q" || it->rank == "K")
        {
            ++count;
        }
        ++it;  // move iterator forward
    }

    return count;
}

bool isWinningHand(const Player& player, int dealerValue) {
    int pValue = player.handValue();
    return (pValue <= 21) && (pValue > dealerValue || dealerValue > 21);
}

// STL CHECKOFF: Output iterator
list <Player> extractWinners(const list<Player>& players, int dealerValue)
{
    list<Player> winners;
    copy_if(players.begin(), players.end(), back_inserter(winners),
        [dealerValue](const Player&p) { return isWinningHand(p, dealerValue);
    });

    return winners;
}

/* --- Eldritch Triggered Function --- */
bool isEldritchTriggered(const Player& player) 
{
    return (player.wins >= 9 || player.balance == 13 || player.balance == 666 || player.balance == 9999);
}
/* --- End of Eldritch Triggered Function --- */
