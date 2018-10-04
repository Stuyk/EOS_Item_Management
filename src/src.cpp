#include "src.hpp"

namespace Inventory {
    // Used to distribute items from the contract holder.
    void Items::issueitem( account_name account, uint64_t key, string name, string type, string memo ) {
        // Only the contract owner can distribute items.
        require_auth( _self );

        // Ensure the items cannot be distributed to the contract. Not that it matters.
        eosio_assert( account != _self, "Cannot distribute items to yourself.");

        // Find the account.
        auto playeracc = _accounts.find( account );
        bool newAccount = false;

        // Create the account if it doesn't exist.
        if (playeracc == _accounts.end()) {
            _accounts.emplace( _self, [&](auto& x) {
                x.owner = account;
            });
            newAccount = true;
        }

        // Run the iterator again so we can pull the new account if necessary.
        if (newAccount) {
            playeracc = _accounts.find( account );
        }

        // Ensure the account exists before we go any further.
        eosio_assert( playeracc != _accounts.end(), "Account does not exist.");

        // Modify the account we've found with the new information.
        _accounts.modify( playeracc, _self, [&](auto& x) {
            // Create a new item based on our parameters and add it to the items array.
            x.items.push_back(item {
                key,
                name,
                type
            });
        });
    }

    void Items::useitem( account_name account, uint64_t key, string memo ) {
        // Only allow the account owner to remove items from itself.
        require_auth( account );

        // Get the player account.
        auto playeracc = _accounts.find( account );

        // Ensure the account exists.
        eosio_assert(playeracc != _accounts.end(), "Account does not exist.");

        // X is our place-holder for accessing the table.
        _accounts.modify(playeracc, account, [&](auto& x) {
            auto foundItem = find_if(x.items.begin(), x.items.end(), [=] (item const& itemHelper) {
                return (itemHelper.key == key);
            });

            eosio_assert(foundItem != x.items.end(), "Item not found.");

            int index = distance(x.items.begin(), foundItem);
            pushqueue(account, x.items.at(index));
            x.items.erase(x.items.begin() + index);
        });
    }

    void Items::pushqueue( account_name account, item useditem ) {
        // Get the account and use a boolean to control if it's a new account.
        auto playeracc = _queue.find( account );
        bool newAccount = false;

        // Check if an account in the queue section exists.
        if (playeracc == _queue.end()) {
            _queue.emplace( _self, [&](auto& x) {
                x.owner = account;
                newAccount = true;
            });
        }

        // Check the item queue again for the player's account.
        if (newAccount)
            playeracc = _queue.find( account );

        // Add our new item to the queue.
        _queue.modify( playeracc, account, [&](auto& x) {
            x.items.push_back(useditem);
        });
    }

    void Items::popqueue( account_name account, uint64_t key, string memo ) {
        // Require contract owner to pop from queue.
        require_auth( _self );

        // Get the account of the player.
        auto playeracc = _queue.find(account);

        // Check if the player has items in the queue.
        eosio_assert(playeracc != _queue.end(), "Player has no items in queue.");

        // If they have items we're going to try and find the item.
        _queue.modify(playeracc, _self, [&](auto& x) {
            auto foundItem = find_if(x.items.begin(), x.items.end(), [=] (item const& itemHelper) {
                return (itemHelper.key == key);
            });

            // We use the key to find the item specifically.
            eosio_assert(foundItem != x.items.end(), "Item not found.");

            int index = distance(x.items.begin(), foundItem);
            x.items.erase(x.items.begin() + index);
        });
    }

    // Clear the table depending on the string called.
    void Items::clear( string tablename, string memo ) {
        require_auth( _self );

        eosio_assert((tablename == accountstable) || (tablename == queuetable), "Invalid name. Try: accounts or queue.");

        // Clear Accounts Table
        if (tablename == accountstable) {
            auto accs = _accounts.begin();
            while (accs != _accounts.end()) {
                accs = _accounts.erase(accs);
            }
        }

        // Clear Queue Table
        if (tablename == queuetable) {
            auto accs = _queue.begin();
            while (accs != _queue.end()) {
                accs = _queue.erase(accs);
            }
        }
    }
}