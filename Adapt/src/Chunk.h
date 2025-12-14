class cChunk {
public:
    cChunk() { // called when a cChunk object is created
        
    }
    void load() {
        
    }

private:
    
};

int main() {
    cChunk chunk; // creates a cChunk object called 'chunk', and automatically calls cChunk::cChunk()

    chunk.load(); // call class-member function
}