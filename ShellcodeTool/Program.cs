using PeNet;
using System;
using System.Diagnostics;
using System.IO;
using System.Linq;
using System.Text;
using System.Threading;

namespace ShellcodeTool
{
    class Program
    {
        private const string path = @"E:\source\repos\FNameToStringEx\ShellcodeTool\cpp\";
        private const string fileName = "c-shellcode";

        private const string headerP1 = @"
#pragma once
#include <vector>

 namespace Shellcode
 {
    constexpr int32_t POINTER_OFFSET = ";

        private const string headerP2
        = @";

    inline std::vector<unsigned char> bytes = { 
    0x";

        private const string headerP3
        = @" };

    void updatePointer(void* address, int32_t offsetOfAddress)
    {
      *reinterpret_cast<uint64_t*>(
          reinterpret_cast<uintptr_t>(bytes.data()) + offsetOfAddress) = (uint64_t) address;
    }
  }
  ";

        static int StartBuildingScript(string s)
        {
            var cl = new Process();

            cl.StartInfo.WorkingDirectory = path;
            cl.StartInfo.FileName = "cmd.exe";
            cl.StartInfo.Arguments = @"/c" + path + s + ".bat";
            cl.StartInfo.UseShellExecute = false;
            cl.StartInfo.RedirectStandardOutput = true;

            cl.Start();

#if DEBUG
            using (StreamReader reader = cl.StandardOutput)
            {
                string result = reader.ReadToEnd();
                Console.Write(result);
            }
#endif

            cl.WaitForExit();

            return cl.ExitCode;
        }

        static void Main(string[] args)
        {
            //CPP -> ASM
            int exitCode; if ((exitCode = StartBuildingScript("build1")) != 0)
            {
                Console.WriteLine("Expected CL to return 0 but it returned {0}", exitCode);
                return;
            }

            string fileContent = File.ReadAllText(path + fileName + ".asm");

            fileContent = fileContent.Replace("INCLUDELIB LIBCMT", string.Empty);
            fileContent = fileContent.Replace("INCLUDELIB OLDNAMES", string.Empty);

            var remove1 = fileContent.IndexOf(";	COMDAT pdata");
            remove1 = remove1 == -1 ? fileContent.IndexOf("pdata	SEGMENT") : remove1;
            var remove2 = fileContent.IndexOf("; Function compile flags: /Odtp");
            fileContent = remove1 != -1 ? fileContent.Remove(remove1, remove2 - remove1) : fileContent;

            //Fix syntax error :skull:
            fileContent = fileContent.Replace("gs:96", "gs:[96]");

            File.WriteAllText(path + fileName + ".asm", fileContent);

            //ASM -> EXE
            if ((exitCode = StartBuildingScript("build2")) != 0)
            {
                Console.WriteLine("Expected ML64 to return 0 but it returned {0}", exitCode);
                return;
            }

            //EXE -> HEADER FILE
            var peFile = new PeFile(path + fileName + ".exe");

            var textSection = (from section in peFile.ImageSectionHeaders where section.Name == ".text" select section).FirstOrDefault();

            if (textSection != null)
            {
                var ptr = (int)textSection.PointerToRawData;
                var size = (int)textSection.SizeOfRawData;
                var rawFile = peFile.RawFile.ToArray();

                byte[] bytes = new byte[size];
                Buffer.BlockCopy(rawFile, ptr, bytes, 0, size);

                int pointerOffset = Array.FindIndex(bytes, element => element == 0xD1);

                var bytesStr = BitConverter.ToString(bytes).Replace("-", ", 0x");

                var sb = new StringBuilder();
                sb.Append(headerP1);
                sb.Append(pointerOffset);
                sb.Append(headerP2);
                sb.Append(bytesStr);
                sb.Append(headerP3);

                File.WriteAllText("shellcode.h", sb.ToString());
                Console.WriteLine("Wrote shellcode header file.");
            }
            else
            {
                Console.WriteLine("Couldn't get the text section from this pe file.");
            }

            Console.WriteLine("Exiting in 5 seconds.");

            Thread.Sleep(5000);
        }
    }
}
