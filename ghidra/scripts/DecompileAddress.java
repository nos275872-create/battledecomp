// Ghidra script to decompile a function or address and export C pseudocode
// @category Decompilation
// @author battledecomp

import ghidra.app.decompiler.DecompileOptions;
import ghidra.app.decompiler.DecompInterface;
import ghidra.app.decompiler.DecompileResults;
import ghidra.app.script.GhidraScript;
import ghidra.program.model.address.Address;
import ghidra.program.model.listing.Function;

import java.io.File;
import java.io.PrintWriter;

public class DecompileAddress extends GhidraScript {
    @Override
    public void run() throws Exception {
        String[] args = getScriptArgs();
        if (args.length == 0) {
            println("Uso: DecompileAddress <hex_address>");
            return;
        }

        String addrStr = args[0];
        Address addr = toAddr(addrStr);
        if (addr == null) {
            printerr("Dirección inválida: " + addrStr);
            return;
        }

        Function func = currentProgram.getFunctionManager().getFunctionAt(addr);
        if (func == null) {
            func = currentProgram.getFunctionManager().getFunctionContaining(addr);
        }
        if (func == null) {
            println("Creando función en " + addr.toString() + "...");
            func = createFunction(addr, "func_" + addrStr);
        }

        if (func == null) {
            printerr("No se pudo obtener ni crear función en: " + addrStr);
            return;
        }

        println("Decompilando: " + func.getName() + " en " + func.getEntryPoint());

        DecompInterface decomp = new DecompInterface();
        decomp.setOptions(new DecompileOptions());
        boolean openOk = decomp.openProgram(currentProgram);
        println("DecompInterface openProgram: " + openOk);

        DecompileResults res = decomp.decompileFunction(func, 60, monitor);

        if (!res.decompileCompleted()) {
            printerr("Fallo de decompilación. Error=" + res.getErrorMessage());
            return;
        }

        String cCode = res.getDecompiledFunction().getC();

        File ghidraDir = getProjectRootFolder().getProjectLocator().getProjectDir().getParentFile().getParentFile();
        File outDir = new File(ghidraDir, "exports/decompiled");
        if (!outDir.exists()) {
            outDir.mkdirs();
        }

        File outFile = new File(outDir, func.getName() + "_" + addrStr + ".c");
        PrintWriter pw = new PrintWriter(outFile);
        pw.println("// Decompilación generada por Ghidra Allegrex");
        pw.println("// Dirección: " + func.getEntryPoint().toString());
        pw.println("// Función: " + func.getName());
        pw.println();
        pw.print(cCode);
        pw.close();

        println("Código decompilado guardado en: " + outFile.getAbsolutePath());
    }
}
