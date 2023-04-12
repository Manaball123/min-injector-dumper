import os
import hashlib
import shutil


membuf_hashtable = {

}

 
def prune_buf_dumps(path : str):
    #iterate over all items
    #there should only be filenames
    old_path = path + "/"
    new_path = path + "-pruned/"
    for fname in os.listdir(old_path):
    
        with open(old_path + fname, "rb") as f:
            bytes = f.read() # read entire file as bytes
            hash = hashlib.sha256(bytes).hexdigest();
        if(hash in membuf_hashtable):
            print(fname + " is a duplicate of " + membuf_hashtable[hash] + ", file is pruned.")
            continue
        membuf_hashtable[hash] = fname

    os.mkdir(new_path)
    
    for v in membuf_hashtable.items():
        shutil.copy(old_path + v, new_path + v)
    


prune_buf_dumps(input())
input()





