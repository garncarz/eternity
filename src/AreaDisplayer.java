/**
 * @file AreaDisplayer.java
 * Obsahuje tridu pro zobrazovani plochy
 */ 

package areaDisplayer;

import java.awt.BorderLayout;
import java.awt.FileDialog;
import java.awt.Event;
import java.awt.Image;
import java.awt.Toolkit;
import java.awt.datatransfer.DataFlavor;
import java.awt.datatransfer.Transferable;
import java.awt.datatransfer.UnsupportedFlavorException;
import java.awt.event.ActionEvent;
import java.awt.event.ActionListener;
import java.awt.event.KeyEvent;
import java.io.File;
import java.io.IOException;
import java.util.List;

import javax.swing.ImageIcon;
import javax.swing.JComponent;
import javax.swing.JFrame;
import javax.swing.JLabel;
import javax.swing.JMenu;
import javax.swing.JMenuBar;
import javax.swing.JMenuItem;
import javax.swing.JOptionPane;
import javax.swing.KeyStroke;
import javax.swing.TransferHandler;

/**
 * Zobrazovac plochy
 * @author Ondra
 */
public class AreaDisplayer extends JFrame {

    /**
     * ???
     */
    private static final long serialVersionUID = 1L;

    /**
     * plocha
     */
    private Area area;
    
    /**
     * posledne nacteny soubor
     */
    private File lastFile;

    /**
     * Vytvori menu
     */
    private void createMenu() {
        JMenuBar bar = new JMenuBar();

        JMenu menu = new JMenu("Soubor");
        bar.add(menu);
        menu.setMnemonic('S');

        JMenuItem loadMenuItem = new JMenuItem("Otevri...");
        menu.add(loadMenuItem);
        loadMenuItem.setMnemonic('O');
        // Ctrl+O
        loadMenuItem.setAccelerator(KeyStroke.getKeyStroke(KeyEvent.VK_O,
                Event.CTRL_MASK));
        loadMenuItem.addActionListener(new ActionListener() {
            public void actionPerformed(ActionEvent e) {
                openFileDialog();
            }
        });

        JMenuItem reloadMenuItem = new JMenuItem("Nacti znovu");
        menu.add(reloadMenuItem);
        reloadMenuItem.setMnemonic('N');
        // F5
        reloadMenuItem.setAccelerator(KeyStroke.getKeyStroke(KeyEvent.VK_F5,
                0));
        reloadMenuItem.addActionListener(new ActionListener() {
            public void actionPerformed(ActionEvent e) {
                reopen();
            }
        });
        
        JMenuItem copyToClipboardMenuItem = new JMenuItem(
                "Zkopiruj jako obrazek do schranky");
        menu.add(copyToClipboardMenuItem);
        copyToClipboardMenuItem.setMnemonic('S');
        // Ctrl+C
        copyToClipboardMenuItem.setAccelerator(KeyStroke.getKeyStroke(
                KeyEvent.VK_C, Event.CTRL_MASK));
        copyToClipboardMenuItem.addActionListener(new ActionListener() {
            public void actionPerformed(ActionEvent e) {
                copyToClipboard();
            }
        });

        JMenuItem saveMenuItem = new JMenuItem("Uloz jako obrazek...");
        menu.add(saveMenuItem);
        saveMenuItem.setMnemonic('U');
        // Ctrl+S
        saveMenuItem.setAccelerator(KeyStroke.getKeyStroke(KeyEvent.VK_S,
                Event.CTRL_MASK));
        saveMenuItem.addActionListener(new ActionListener() {
            public void actionPerformed(ActionEvent e) {
                saveImageDialog();
            }
        });
        
        JMenuItem exitMenuItem = new JMenuItem("Konec");
        menu.add(exitMenuItem);
        exitMenuItem.setMnemonic('K');
        // Esc
        exitMenuItem.setAccelerator(KeyStroke.getKeyStroke(KeyEvent.VK_ESCAPE,
                0));
        exitMenuItem.addActionListener(new ActionListener() {
            public void actionPerformed(ActionEvent e) {
                System.exit(0);
            }
        });

        setJMenuBar(bar);
    }

    /**
     * Otevre dialog s chybovou hlaskou
     * @param message text chyby
     */
    public void error(String message) {
        JOptionPane.showMessageDialog(this, message, "Chyba",
                JOptionPane.ERROR_MESSAGE);
    }

    /**
     * Otevre dialog pro otevreni souboru s daty plochy
     * a zpracuje vysledek
     */
    public void openFileDialog() {
        FileDialog fd = new FileDialog(this,
                "Vyberte soubor s daty plochy",
                FileDialog.LOAD);

        fd.setFile("*.area");
        // fd.setDirectory(".");

        fd.setVisible(true);

        if (fd.getFile() != null) {
            try {
                File file = new File(fd.getDirectory() + fd.getFile());
                open(file);
            } catch (Exception e) {
                error("Chyba pri nacitani souboru.\n" +
                        "Maji vsechny symboly obrazek v adresari img?");
                e.printStackTrace();
            }
        }
        else if (area.getImage() == null)  // pokud neni nactena zadna plocha,
            System.exit(0);                // ukoncime program
    }
    
    /**
     * Nacte znovu posledne otevreny soubor
     */
    public void reopen() {
        if (lastFile != null) {
            try {
                open(lastFile);
            } catch (Exception e) {
                error("Chyba pri nacitani souboru.\n" +
                        "Maji vsechny symboly obrazek v adresari img?");
                e.printStackTrace();
            }
        }
    }

    /*
     * JFileChooser => moc pomale
    public void openFileDialog() {
        JFileChooser fc = new JFileChooser();
        fc.setDialogTitle("Vyberte soubor s daty plochy");
        fc.setCurrentDirectory(new File("."));

        fc.addChoosableFileFilter(new FileFilter() {
            public boolean accept(File f) {
                if (f.isDirectory() ||
                        f.getName().toLowerCase().endsWith(".area"))
                    return true;
                return false;
            }
            public String getDescription() {
                return "Data plochy (*.area)";
            }
        });

        if (fc.showOpenDialog(this) == JFileChooser.APPROVE_OPTION) {
            File file = fc.getSelectedFile();
            try {
                open(file);
            } catch (Exception e) {
                error("Chyba pri nacitani souboru.");
            }
        }
    }*/

    /**
     * Otevre dialog pro ulozeni obrazku plochy do souboru
     * a zpracuje vysledek
     */
    public void saveImageDialog() {
        if (area.getImage() == null)
            return;

        FileDialog fd = new FileDialog(this,
                "Uloz jako PNG obrazek...",
                FileDialog.SAVE);

        fd.setFile("*.png");
        // fd.setDirectory(".");

        fd.setVisible(true);

        if (fd.getFile() != null) {
            try {
                File file = new File(fd.getDirectory() + fd.getFile());
                saveImage(file);
            } catch (Exception e) {
                error("Chyba pri ukladani obrazku.");
            }
        }
    }

    /*
     * JFileChooser => moc pomale
    public void saveImageDialog() {
        if (area.getImage() == null)
            return;

        JFileChooser fc = new JFileChooser();
        fc.setDialogTitle("Uloz jako PNG obrazek...");
        fc.setCurrentDirectory(new File("."));

        fc.addChoosableFileFilter(new FileFilter() {
            public boolean accept(File f) {
                if (f.isDirectory() ||
                        f.getName().toLowerCase().endsWith(".png"))
                    return true;
                return false;
            }
            public String getDescription() {
                return "PNG obrazek (*.png)";
            }
        });

        if (fc.showSaveDialog(this) == JFileChooser.APPROVE_OPTION) {
            File file = fc.getSelectedFile();
            try {
                saveImage(file);
            } catch (Exception e) {
                error("Chyba pri ukladani obrazku.");
            }
        }
    }*/
    
    /**
     * Zkopiruje obrazek plochy do schranky
     */
    public void copyToClipboard() {
        if (area.getImage() == null)
            return;

        ImageTransferable it = new ImageTransferable(area.getImage());
        Toolkit.getDefaultToolkit().getSystemClipboard().setContents(it, null);
    }

    /**
     * Nacte data plochy z textoveho souboru,
     * vygeneruje obrazek a zobrazi jej
     * @param file textovy soubor
     * @throws IOException chyba prace se souborem
     */
    public void open(File file) throws IOException {
        lastFile = file;
        area.loadAreaData(file);
        area.generate();
        JLabel image = new JLabel(new ImageIcon(area.getImage()));
        getContentPane().removeAll();
        getContentPane().add(image, BorderLayout.CENTER);
        setTitle(file.getName());  // nastavi jmeno souboru do titulku okna
        pack();
        System.out.println(file);
        area.write();
    }
    
    /**
     * Ulozi obrazek plochy do souboru
     * @param file soubor
     * @throws IOException chyba prace se souborem
     */
    public void saveImage(File file) throws IOException {
        if (!file.getName().toLowerCase().endsWith(".png"))
            file = new File(file + ".png");
        area.saveImage(file);
    }

    /**
     * Konstruktor,
     * zinicializuje frame
     * @param inputFilename jmeno souboru s daty plochy pro zobrazeni
     */
    public AreaDisplayer(String inputFilename) {
        super("AreaDisplayer");
        setDefaultCloseOperation(JFrame.EXIT_ON_CLOSE);
        setLocationByPlatform(true);

        // zaregistruje instanci tridy pro zachytavani drag'n'drop souboru
        ((JComponent)getContentPane()).setTransferHandler(
                new DragAndDropHandler(this));

        createMenu();

        pack();
        setVisible(true);

        // inicializace plochy
        try {
            area = new Area();
        } catch (Exception e) {
            error("Chyba pri inicializaci herniho pole.\n" +
                    "Existuje adresar img s obrazky symbolu?");
            System.exit(0);
        }

        // nacteni dat
        try {
            open(new File(inputFilename));
        } catch (Exception e) {
            openFileDialog();
        }
    }

    /**
     * Spousti zobrazovac
     * @param args nepovinne parametry:
     *      args[0] - jmeno input souboru s daty plochy;
     *      args[1] - jmeno output souboru, kde bude ulozen obrazek plochy
     */
    public static void main(String[] args) {
        String inputFilename = "area.area";
        if (args.length >= 1)
            inputFilename = args[0];
        if (args.length >= 2) {
            // v pripade vytvoreni obrazku plochy se pouze pokusime dany
            // obrazek vytvorit, nebudeme pouzivat GUI a nic zobrazovat
            String outputFilename = args[1];
            try {
                Area area = new Area();
                area.loadAreaData(new File(inputFilename));
                area.generate();
                area.saveImage(new File(outputFilename));
            } catch (Exception e) {
                e.printStackTrace();
                System.exit(1);
            }
            return;
        }
        new AreaDisplayer(inputFilename);
    }
    
}


/**
 * Trida pro drag'n'drop souboru (souboru hraci plochy)
 * @author Ondra
 */
class DragAndDropHandler extends TransferHandler {

    /**
     * zobrazovac plochy
     */
    private AreaDisplayer displayer;

    /**
     * Konstruktor, zapamatuje si zobrazovac plochy
     * @param displayer zobrazovac plochy
     */
    public DragAndDropHandler(AreaDisplayer displayer) {
        this.displayer = displayer;
    }

    /**
     * Muzeme naimportovat pretazena data?
     * @return ano :-)
     */
    public boolean canImport(JComponent comp, DataFlavor[] data) {
        return true;
    }

    /**
     * Naimportuje pretazena data (soubor s plochou)
     * @param comp komponenta, kam byla data pretazena (nepouziva se)
     * @param trans pretazena data
     * @return uspech importovani dat
     */
    public boolean importData(JComponent comp, Transferable trans) {
        try {
            // ziskame soubor z pretazenych dat
            File file = (File)(((List)trans.getTransferData(
                    DataFlavor.javaFileListFlavor)).iterator().next());
            // nechame zobrazovac zobrazit dany soubor
            displayer.open(file);
        } catch (Exception e) { // chyba - zrejme nespravny soubor
            e.printStackTrace();
            return false;
        }
        return true;
    }

}


/**
 * Trida pro zkopirovani obrazku plochy do schranky
 * @author Ondra
 */
class ImageTransferable implements Transferable {

    /**
     * obrazek
     */
    private Image image;
    
    /**
     * Konstruktor, ulozi si obrazek
     * @param image obrazek plochy
     */
    ImageTransferable(Image image) {
        this.image = image;
    }
    
    /**
     * Vraci typy dat uchovanych v tomto objektu - pro nas pripad obrazky
     * @return typy dat
     */
    public DataFlavor[] getTransferDataFlavors() {
        return new DataFlavor[] {DataFlavor.imageFlavor};
    }
    
    /**
     * Vraci, zda-li je dany typ dat timto objektem podporovan
     * @param flavor typ dat
     * @return je podporovan?
     */
    public boolean isDataFlavorSupported(DataFlavor flavor) {
        return DataFlavor.imageFlavor.equals(flavor);
    }
    
    /**
     * Vraci data uchovavana timto objektem
     * @param flavor pozadovany typ/format dat
     * @return data
     */
    public Object getTransferData(DataFlavor flavor)
            throws UnsupportedFlavorException, IOException {
        return image;
    }
    
}
