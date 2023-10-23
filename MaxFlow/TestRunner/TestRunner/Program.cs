using System;
using System.Collections;
using System.Collections.Generic;
using System.Diagnostics;
using System.IO;
using System.Linq;
using System.Text;
using System.Threading;
using System.Threading.Tasks;
using System.Management;
using System.Windows.Input;

namespace TestRunner
{
    class Program
    {
        private static ManagementClass mcWin32;
        private static ManagementBaseObject mboShutdownParams;

        private const string executable = @"D:\Users\Beier\Documents\MasterThesis\MaxFlow\x64\Release\MaxFlow.exe";
        static readonly string[] algorithms = new[]
                                 {
                                     "EK",
                                     "Dinic",
                                     "GT N3",
                                     "GT Dyn",
                                     "KR",
                                     "KR Mod",
                                     "KR Mod Dyn",
                                     "GR", 
                                     "GT N3 GR",
                                     "GT D GR",
                                     "KR1992 GR",
                                     "KR1992 M GR",
                                     "KR1992 M D GR",
                                     "GT N3 RGR",
                                     "GT D RGR",
                                     "KR1992 RGR",
                                     "KR1992 M RGR",
                                     "KR1992 M D RGR",
                                     "GT GRE2",
                                     "GT D GRE2",
                                     "KR LM GRE2",
                                     "KR LM D GRE2",
                                 };
        static readonly string[] graphTypes = new[]
                                 {
                                     "CBC",
                                     "CWC",
                                     "AK",
                                     "GenRmf b=aa",
                                     "GenRmf a=bb",
                                     "GenRmf a=b",
                                     "Wash a=64",
                                     "Wash b=64",
                                     "Bad Dinic"
                                 };
        const int maxDuration = 60 * 20;
        static readonly string folder = DateTimeOffset.UtcNow.Year + "-" + DateTimeOffset.UtcNow.Month + "-" + DateTimeOffset.UtcNow.Day;
        private static bool stop;

        [STAThread]
        static void Main(string[] args)
        {

            mcWin32 = new ManagementClass("Win32_OperatingSystem");
            mcWin32.Get();

            // You can't shutdown without security privileges
            mcWin32.Scope.Options.EnablePrivileges = true;
            mboShutdownParams = mcWin32.GetMethodParameters("Win32Shutdown");

            

            Directory.CreateDirectory(folder);



            /*Test(new[]
                     {
                        "GT GRE2",
                        "GT D GRE2",
                        "KR LM GRE2",
                        "KR LM D GRE2"
                     },
                     graphTypes);*/


            Test(new[]
                     {
                         "GT D RGR",
                         "KR1992 M RGR",
                         "KR1992 M D RGR",
                     },
                 new[]
                     {
                         "GenRmf a=b"
                     });

            Test(new[]
                     {
                         "GT Dyn",
                         "KR Mod",
                         "KR Mod Dyn",
                         "GT D GR",
                         "KR1992 M GR",
                         "KR1992 M D GR",
                         "GT D RGR",
                         "KR1992 M RGR",
                         "KR1992 M D RGR",
                     },
                 new[]
                     {
                         "Wash a=64",
                         "Wash b=64",
                         "Bad Dinic"
                     });

            if (stop)
                return;
            
            Process.Start("shutdown", "/s /t 0");
            Thread.Sleep(10000);
            Shutdown();
        }


        static void Test(string algorithmFrom, string algorithmTo, string graphFrom, string graphTo)
        {
            Test(Array.IndexOf(algorithms, algorithmFrom),
                 Array.IndexOf(algorithms, algorithmTo),
                 Array.IndexOf(graphTypes, graphFrom),
                 Array.IndexOf(graphTypes, graphTo));
        }

        static void Test(int algorithm, int graph)
        {
            Test(algorithm, algorithm, graph, graph);
        }

        
        static void Test(int algorithmFrom, int algorithmTo, int graphFrom, int graphTo)
        {
            Test(Enumerable.Range(algorithmFrom, algorithmTo - algorithmFrom + 1),
                 Enumerable.Range(graphFrom, graphTo - graphFrom + 1));
        }


        static void Test(IEnumerable<string> algorithmsToTest, IEnumerable<string> graphsToTest)
        {
            Test(algorithmsToTest.Select(x => Array.IndexOf(algorithms, x)),
                graphsToTest.Select(x => Array.IndexOf(graphTypes, x)));
        }

        static void Test(IEnumerable<int> algorithmsToTest, IEnumerable<int> graphsToTest)
        {
            foreach (var i in graphsToTest)
            {
                foreach (var j in algorithmsToTest)
                {
                    if (stop)
                        return;
                    string outputFile = folder + "\\" + graphTypes[i] + "_" + algorithms[j] + ".csv";
                    var arguments = string.Format("{0} {1} {2} \"{3}\"", i, j, maxDuration, outputFile);
                    var process = Process.Start(executable, arguments);

                    var timeout = DateTimeOffset.UtcNow + TimeSpan.FromMinutes(15);
                    while (!process.HasExited)
                    {
                        Console.Clear();
                        Console.WriteLine("Testing " + algorithms[j] + " on " + graphTypes[i]);
                        var remaining = timeout - DateTimeOffset.UtcNow;
                        Console.WriteLine("Remaining: " + remaining.ToString("mm\\:ss"));
                        if (timeout <= DateTimeOffset.UtcNow)
                            process.Kill();
                        if (Keyboard.IsKeyDown(Key.Return))
                        {
                            while (Keyboard.IsKeyDown(Key.Return))
                                Thread.Sleep(10);
                            Console.ReadKey();
                            Console.WriteLine("Press Q to stop after this run");
                            var line = Console.ReadLine();
                            if (line.ToLower() == "q")
                                stop = true;
                        }
                        if (stop)
                            Console.WriteLine("Stopping after this run.");
                        Thread.Sleep(100);
                    }
                }
            }
        }


        static void Shutdown()
        {
            // Flag 1 means we want to shut down the system. Use "2" to reboot.
            mboShutdownParams["Flags"] = "1";
            mboShutdownParams["Reserved"] = "0";
            foreach (ManagementObject manObj in mcWin32.GetInstances())
            {
                manObj.InvokeMethod("Win32Shutdown", mboShutdownParams, null);
            }
        }
    }
}
