You have a bunch of files that you want to search efficiently.
So, you have code that reads them over, storing the connections between words and filenames.
This requires you to have a connector between the words and the filenames.
Additionally, you have code to save the connections and load them so that the files don't all need to be reread.
From there, you have a search engine that uses those connections to allow you to search.
Finally, you create a user interface for that search engine.

So, you have:
A wordmap that allows for inserting, removing, saving, and loading.
A search engine that loads in its constructor and saves in its destructor.
If the search engine cannot load in its constructor, it reads all the files, building the wordmap from the ground up.
A GUI similar to google that allows you to search for files in your search engine.