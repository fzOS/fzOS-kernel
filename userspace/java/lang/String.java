package java.lang;
public class String {
    private byte[] internBytes;
    public String(byte[] bytes) {
        this.internBytes = bytes;
    }
}
