# EOS_Inventory_Manager
EOS Smart Contract for creating a blockchain based inventory.

### Basic Idea and Use Case
Let's say you have a video game that wants to use EOS for managing a user's inventory. Here's a general way of how this could potentially be used.

* Issue an Item to an Account
* User Consumes Item In-Game
* Blockchain Moves Item to Queue
* Server checks the queue for new consumption transactions
* If there is a new consumption item in the queue have your server send a transaction out.
* Remove the item from the queue
* Issue the item in-game / use it / etc.

#### Using the Contract
(Anything in 'camelCasing' is a variable)

ie. accountName = YOUR ACCOUNT NAME

#### Get the accounts table to see all items:
```
cleos get table accountName accountName accounts
```

#### Get the queue table to see all queue'd items.
```
cleos get table accountName accountName queue
```

#### Setup and issue an acccount an item:
Action:
* issueitem

Properties:
* account
* key
* itemname
* type
* memo
```
cleos push action accountName issueitem '["accountName", 12345, "Fish", "Food", "Issued Item"]' -p accountName
```

#### Consume Items:
Action:
* useitem

Properties:
* account
* key
* memo
```
cleos push action accountName useitem '["targetAccount", 12345, "Used Item"]' -p targetAccount
```

### Pop from Queue:
Action:
* popqueue

Properties:
* account
* key
* memo
```
cleos push action accountName popqueue '["targetAccount", 12345, "Removed from Queue"]' -p accountName
```

### Clear Tables:
Action:
* clear

Properties:
* option
* memo

Options:
* accounts
* queue
```
cleos push action accountName clear '["accounts", "Cleared the table."]' -p accountName
```

