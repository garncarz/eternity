/**
 * @file Area.java
 * Obsahuje tridu pro (grafickou) praci s plochou
 */

package areaDisplayer;

import java.awt.Color;
import java.awt.Graphics;
import java.awt.Graphics2D;
import java.awt.Image;
import java.awt.Toolkit;
import java.awt.RenderingHints;
import java.awt.geom.AffineTransform;
import java.awt.image.BufferedImage;
import java.awt.image.FilteredImageSource;
import java.awt.image.ImageFilter;
import java.awt.image.ImageProducer;
import java.awt.image.RGBImageFilter;
import java.awt.image.RenderedImage;
import java.io.BufferedReader;
import java.io.File;
import java.io.FileReader;
import java.io.InputStreamReader;
import java.io.IOException;
import java.util.HashMap;
import java.util.regex.Matcher;
import java.util.regex.Pattern;

import javax.imageio.ImageIO;

/**
 * Plocha,
 * umi nacist data z textoveho souboru a zpracovat
 * je do formy obrazku
 * @author Ondra
 */
public class Area {

    /**
     * maximalni mozna pocet ctverecku na sirku
     */
    public static final int MAX_X = 16;
    
    /**
     * maximalni mozna pocet ctverecku na vysku
     */
    public static final int MAX_Y = 16;
    
    /**
     * sirka ctverecku v pixelech
     */
    public static final int WIDTH = 64;
    
    /**
     * vyska ctverecku v pixelech
     * (nema moc smysl, melo by byt vzdy rovno WIDTH)
     */
    public static final int HEIGHT = 64;
    
    /**
     * pocet ctverecku na sirku
     */
    private int max_x;
    
    /**
     * pocet ctverecku na vysku
     */
    private int max_y;
    
    /**
     * pocet (nesedych) symbolu
     */
    private int max_symbols;
    
    /**
     * obsah plochy
     * [sloupec][radek][strana]
     */
    private String[][][] area = new String[MAX_X][MAX_Y][];
    
    /**
     * obrazek plochy
     */
    private BufferedImage image;
    
    /**
     * obrazky symbolu
     */
    private HashMap<String, Image> images;
    
    /**
     * Konstruktor
     * @throws IOException nepodarilo se nacist obrazky symbolu
     */
    public Area() throws IOException {
        loadImages();
    }
    
    /**
     * Nacte data plochy z textoveho souboru
     * @param file textovy soubor
     * @throws IOException chyba pristupu k souboru
     */
    public void loadAreaData(File file) throws IOException {
        BufferedReader in = new BufferedReader(new FileReader(file));
        String line;
        
        line = in.readLine();  // rozmery plochy a pocet symbolu
        // pocet symbolu
        max_symbols = Integer.valueOf(line.split("x")[2]).intValue(); 
        in.readLine();  // zafixovane karticky - ignorujeme
        
        // vzor jedne karticky
        Pattern cardPattern = Pattern.compile("\\[[^\\]]*\\]");
        
        int x = 0, y;
        
        for (y = 0; (line = in.readLine()) != null; y++) {
            Matcher cards = cardPattern.matcher(line);
            for (x = 0; cards.find(); x++) {
                String card = line.substring(cards.start() + 1,
                        cards.end() - 1);
                area[x][y] = card.split(",");
            }
        }
        
        max_x = x;
        max_y = y;
        
        in.close();
    }
    
    /**
     * Vypise plochu na standardni vystup
     */
    public void write() {
        for (int y = 0; y < max_y; y++) {
            for (int x = 0; x < max_x; x++) {
                System.out.print("[" + area[x][y][0] + "," +
                        area[x][y][1] + "," +
                        area[x][y][2] + "," +
                        area[x][y][3] + "] ");
            }
            System.out.println();
        }
        System.out.println();
    }
    
    /**
     * Nacte obrazky symbolu
     * @throws IOException chyba prace se soubory obrazku ci adresarem
     */
    private void loadImages() throws IOException {
        images = new HashMap<String, Image>();
        
        try {
            // zkusime precist obrazky z resourcu:
            BufferedReader br = new BufferedReader(new InputStreamReader(
              getClass().getResourceAsStream("/img/list.txt")));
            
            while (br.ready()) {
                String name = br.readLine();
                try {
                    Image image = ImageIO.read(
                            getClass().getResourceAsStream("/img/" + name));
                    image = transparent(image); // zpruhlednime bilou
                    image = resize(image);  // normalizujeme velikost
                    name = name.substring(0, name.lastIndexOf('.'));
                    images.put(name, image);
                }
                catch (Exception imgExc) {
                    // ne kazdy soubor v adresari musi byt obrazek
                    // - kvuli Thumbs.db
                }
            }
        } catch (Exception e) {
            // kdyz nevyjde cteni z resourcu, tak se pokusime obrazky
            // nacist z adresare img
            File folder = new File("img");
            File[] files = folder.listFiles();
            
            for (int i = 0; i < files.length; i++) {
                try {
                    File file = files[i];
                    String name = file.getName();
                    name = name.substring(0, name.lastIndexOf('.'));
                    Image image = ImageIO.read(file);
                    image = transparent(image); // zpruhlednime bilou
                    image = resize(image);  // normalizujeme velikost
                    images.put(name, image);
                }
                catch (Exception imgExc) {
                    // ne kazdy soubor v adresari musi byt obrazek
                    // - kvuli Thumbs.db
                }
            }
        }
    }
    
    /**
     * Vraci obrazek, kde puvodni bila barva je ucinena pruhlednou,
     * prevzato z http://www.rgagnon.com/javadetails/java-0265.html
     * @param image puvodni obrazek
     * @return obrazek s pruhlednou barvou
     */
    public static Image transparent(Image image) {
        final Color color = Color.WHITE;
        
        ImageFilter filter = new RGBImageFilter() {
            public int markerRGB = color.getRGB() | 0xFF000000;
            public final int filterRGB(int x, int y, int rgb) {
                if ((rgb | 0xFF000000) == markerRGB)
                    return 0x00FFFFFF & rgb;
                else return rgb;
            }
        };
        
        ImageProducer ip = new FilteredImageSource(image.getSource(), filter);
        return Toolkit.getDefaultToolkit().createImage(ip);
    }
    
    /**
     * Normalizuje velikost obrazku podle atributu WIDTH a HEIGHT
     * @param image puvodni obrazek
     * @return obrazek s normalizovanou velikosti
     */
    public static Image resize(Image image) {
        BufferedImage bi = new BufferedImage(WIDTH, HEIGHT,
                BufferedImage.TYPE_INT_ARGB);
        Graphics2D g = bi.createGraphics();
        g.scale((double)WIDTH / image.getWidth(null),
                (double)HEIGHT / image.getHeight(null));
        g.drawImage(image, 0, 0, null);
        g.dispose();
        return bi;
    }
    
    /**
     * Prevede obrazek typu Image na obrazek BufferedImage
     * @param image obrazek typu Image
     * @return obrazek typu BufferedImage
     */
    public static BufferedImage imageToBufferedImage(Image image)
    {
       BufferedImage bi = new BufferedImage(image.getWidth(null),
               image.getHeight(null),
               BufferedImage.TYPE_INT_ARGB);
       Graphics g = bi.createGraphics();
       g.drawImage(image, 0, 0, null);
       g.dispose();
       return bi;
    }
    
    /**
     * Vygeneruje obrazek plochy do atributu image
     */
    public void generateImage() {
        image = new BufferedImage(max_x * WIDTH + 1,
                max_y * HEIGHT + 1, BufferedImage.TYPE_INT_ARGB);
        Graphics2D total = image.createGraphics();

        // pro vsechny radky
        for (int y = 0; y < max_y; y++) {
            // pro vsechny sloupce
            for (int x = 0; x < max_x; x++) {
                // pro vsechny strany
                for (int side = 0; side < 4; side++) {
                    // obrazek daneho symbolu (v NORTH smeru)
                    String imageName = area[x][y][side];
                    if (Integer.valueOf(imageName).intValue() > max_symbols)
                        imageName = "undefined";  // pro neplatne symboly
                    BufferedImage origTriangle =
                            imageToBufferedImage(images.get(imageName));

                    // natocime jej podle strany
                    AffineTransform af = new AffineTransform();
                    af.translate(WIDTH / 2, HEIGHT / 2);
                    af.rotate(side * Math.toRadians(90));
                    af.translate(-WIDTH / 2, -HEIGHT / 2);
                    BufferedImage triangle = new BufferedImage(WIDTH,
                            HEIGHT, BufferedImage.TYPE_INT_ARGB);
                    Graphics2D part = triangle.createGraphics();
                    part.drawImage(origTriangle, af, null);
                    part.dispose();

                    // pridame do celkove plochy tam, kam patri
                    af.setToTranslation(x * WIDTH, y * HEIGHT);
                    total.drawImage(triangle, af, null);
                }
            }
        }
        
        total.dispose();
    }
    
    /**
     * Prida do obrazku mrizky mezi kartickami
     */
    public void generateGrid() {
        Graphics2D g = image.createGraphics();
        g.setColor(Color.BLACK);
        
        for (int x = 0; x <= max_x; x++)
            g.drawLine(x * WIDTH, 0, x * WIDTH, max_y * HEIGHT);
        for (int y = 0; y <= max_y; y++)
            g.drawLine(0, y * HEIGHT, max_x * WIDTH, y * HEIGHT);
        
        g.dispose();
    }
    
    /**
     * Prida do obrazku vnitrni mrizky karticek
     */
    public void generateInnerGrid() {
        Graphics2D g = image.createGraphics();
        g.setColor(Color.BLACK);
        g.setRenderingHint(RenderingHints.KEY_ANTIALIASING,
                RenderingHints.VALUE_ANTIALIAS_ON);
        
        for (int x = 0; x < max_x; x++)
            g.drawLine(x * WIDTH, 0, (x + max_x) * WIDTH, max_x * HEIGHT);
        for (int y = 1; y < max_y; y++)
            g.drawLine(0, y * HEIGHT, max_x * WIDTH, (y + max_x) * HEIGHT);
        
        for (int x = 1; x <= max_x; x++)
            g.drawLine(x * WIDTH, 0, 0, x * HEIGHT);
        for (int y = 1; y < max_y; y++)
            g.drawLine(max_x * WIDTH, y * HEIGHT, 0, (y + max_x) * HEIGHT);
        
        g.dispose();
    }
    
    /**
     * Vygeneruje obrazek i s mrizkami do atributu image
     */
    public void generate() {
        generateImage();
        generateGrid();
        generateInnerGrid();
    }
    
    /**
     * Ulozi obrazek do souboru
     * @param file soubor
     * @throws IOException chyba prace se souborem
     */
    public void saveImage(File file) throws IOException {
        ImageIO.write((RenderedImage)image, "PNG", file);
    }
    
    /**
     * Vraci obrazek plochy
     * @return obrazek plochy
     */
    public Image getImage() {
        return image;
    }
    
}
