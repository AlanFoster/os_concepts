void main() {
    char* video_memory = (char*) 0xb8000;
    char* message = "hello world, from c kernel land!\0";
    int rowWidth = 160;
    int row = rowWidth * 3;

    for (int i = 0 ; message[i] != 0; i++) {
        int column = i * 2;
        *(video_memory + row + column) = message[i];
    }
}
