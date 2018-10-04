#include <eosiolib/eosio.hpp>
#include <eosiolib/print.hpp>
#include <string>
#include <tuple>
#include <algorithm>

namespace Inventory {
    using namespace eosio;
    using std::string;

    class Items : public contract {
        using contract::contract;

        public:
            Items(account_name _self ):contract( _self ), _queue(_self, _self), _accounts(_self, _self) {}

            [[eosio::action]]
            void issueitem( account_name account, uint64_t key, string itemname, string type, string memo );

            [[eosio::action]]
            void useitem( account_name account, uint64_t key, string memo );

            [[eosio::action]]
            void popqueue( account_name account, uint64_t key, string memo);

            [[eosio::action]]
            void clear( string tablename, string memo );

            // Mainly used in our Vector.
            struct [[eosio::table]] item {
                uint64_t key;
                string name;
                string type;

                uint64_t primary_key() const { return key; }

                EOSLIB_SERIALIZE(item, (key)(name)(type))
            };

            // A table containing account owners for the primary key and a list of their assigned items.
            struct [[eosio::table]] accounts {
                account_name owner;
                vector<item> items;

                uint64_t primary_key() const { return owner; }
                EOSLIB_SERIALIZE(accounts, (owner)(items))
            };

            typedef multi_index<N(accounts), accounts> accountindex;

            // A queue that the server will manage to eventually remove items from the system.
            struct [[eosio::table]] queue {
                account_name owner;
                vector<item> items;

                uint64_t primary_key() const { return owner; }
                EOSLIB_SERIALIZE(queue, (owner)(items))
            };

            typedef multi_index<N(queue), queue> queueindex;
        private:
            void pushqueue( account_name account, item useditem );
            const string accountstable = "accounts";
            const string queuetable = "queue";

            accountindex _accounts;
            queueindex _queue;
    };

    EOSIO_ABI(Items, (issueitem)(useitem)(popqueue)(clear))
}