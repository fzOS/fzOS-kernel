package java.lang;
public class String {
    private byte[] internBytes;
    public String(byte[] bytes) {
        this.internBytes = bytes;
    }
    @Override
    public boolean equals(Object o) {
        if(!(o instanceof String)) {
            return false;
        }
        String s = (String)o;
        return Arrays.equals(s.internBytes,this.internBytes);
    }
}
