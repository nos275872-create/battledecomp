// Ghidra script to export functions and memory blocks summary
// @category Analysis
// @author battledecomp

import ghidra.app.script.GhidraScript;
import ghidra.program.model.listing.Function;
import ghidra.program.model.listing.FunctionIterator;
import ghidra.program.model.mem.MemoryBlock;

import java.io.File;
import java.io.PrintWriter;

public class ExportSummary extends GhidraScript {
    @Override
    public void run() throws Exception {
        println("ExportSummary: Iniciando exportación de metadatos...");

        File ghidraDir = getProjectRootFolder().getProjectLocator().getProjectDir().getParentFile().getParentFile();
        File exportDir = new File(ghidraDir, "exports");
        if (!exportDir.exists()) {
            exportDir.mkdirs();
        }

        File outFile = new File(exportDir, "functions.tsv");
        println("Escribiendo funciones a: " + outFile.getAbsolutePath());

        PrintWriter writer = new PrintWriter(outFile);
        writer.println("address\tname\tsize\tentry_point\tbody_min\tbody_max");

        FunctionIterator functions = currentProgram.getFunctionManager().getFunctions(true);
        int count = 0;
        while (functions.hasNext() && !monitor.isCancelled()) {
            Function f = functions.next();
            writer.printf("%s\t%s\t%d\t%s\t%s\t%s\n",
                f.getEntryPoint().toString(),
                f.getName(),
                f.getBody().getNumAddresses(),
                f.getEntryPoint().toString(),
                f.getBody().getMinAddress().toString(),
                f.getBody().getMaxAddress().toString()
            );
            count++;
        }
        writer.close();

        println("ExportSummary completado: " + count + " funciones exportadas.");

        // Export memory blocks
        File memFile = new File(exportDir, "memory_blocks.tsv");
        PrintWriter memWriter = new PrintWriter(memFile);
        memWriter.println("name\tstart\tend\tsize\tread\twrite\texecute\tinitialized");
        for (MemoryBlock block : currentProgram.getMemory().getBlocks()) {
            memWriter.printf("%s\t%s\t%s\t%d\t%b\t%b\t%b\t%b\n",
                block.getName(),
                block.getStart().toString(),
                block.getEnd().toString(),
                block.getSize(),
                block.isRead(),
                block.isWrite(),
                block.isExecute(),
                block.isInitialized()
            );
        }
        memWriter.close();
        println("Bloques de memoria guardados en: " + memFile.getAbsolutePath());
    }
}
