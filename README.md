# businesscard
Smartcontract for minting and updating a NFT-based business card on the WAX-Blockchain / AtomicAssets 

You can mint (and edit) your own NFT-based businesscard here:
https://wax.bloks.io/account/businesscard?loadContract=true&tab=Tables&account=businesscard&scope=businesscard&limit=100

This smartcontract can be used by NFT-game developer als a template for minting NFT's from a smartcontract. You also will learn how to read current mutable attributes, modify and write back to the NFT mutable data.

* How to create an NFT with mutable data (not only immutable) *
1. Go to NFT-Creator on atomichub and create an schema with ALL needed attributes (mutable AND immutable data)
2. Create an template from this schema WITHOUT any predefined values (let everything empty). Remember the resulting template ID
3. Use the 'mintasset()' method (of the atomicassets-contract) in order to define the initial values of the NFT, here you have two seperate parameters for the values for mutable and immutable values

In this example the "mintasset()" - method will be called by the businesscard-smartcontract (YOU WILL FIND THIS IN THIS REPOSITORY, businesscard.cpp!), in the method "mintcard()".

The second method "editcard()" (also in businesscard.cpp) can be called from the businesscard-creator in order to UPDATE the contact-data (immutable-field "info"). If you already send your NFT-businesscard to a customer and/or a friend, you can without the permission of the current owner update your contact data. This may be useful if you have a new company, e-mail, phone-number or a new telegram-ID. This method "editcard()" also will load from the immutable-data a counter with the past number of updates, increment this number und write it back to the NFT. It will also update the update-date, so the user / nft-owner can also check the last time of your update. 

This businesscard-immutable data example can also be useful for GAME-DEVELOPERS who wants to give more value to the NFT's (e.g. an NFT-based RPG-character, where players can UPLEVEL by playing several features, like power, fights, defense, mana or other game-parameters. Highly used Game NFT's will become more worth and the NFT power will increase in time and playing.




