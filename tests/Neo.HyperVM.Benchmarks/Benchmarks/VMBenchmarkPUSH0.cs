﻿using BenchmarkDotNet.Attributes;
using Neo.SmartContract;
using NeoSharp.VM;

namespace Neo.HyperVM.Benchmarks
{
    public class VMBenchmarkPUSH0 : VMBenchmarkBase
    {
        #region Scripts

        byte[] _script;

        #endregion

        [Params("(PUSH0+DROP)*1K")]
        public override string Test { get; set; }

        [GlobalSetup]
        public override void Setup()
        {
            using (var script = new ScriptBuilder())
            {
                for (int x = 0; x < 1000; x++)
                {
                    script.Emit(EVMOpCode.PUSH0);
                    script.Emit(EVMOpCode.DROP);
                }

                _script = script.ToArray();
            }

            base.Setup();
        }

        [Benchmark]
        public override void HyperVM()
        {
            using (var engine = _HyperVM.Create(null))
            {
                engine.LoadScript(_script);
                engine.Execute();
            }
        }

        [Benchmark]
        public override void NeoVM()
        {
            using (var engine = new VM.ExecutionEngine(null, _crypto, null, null))
            {
                engine.LoadScript(_script, -1);
                engine.Execute();
            }
        }

        [Benchmark]
        public override void ApplicationEngine()
        {
            using (var engine = new ApplicationEngine(TriggerType.Application, null, null, Fixed8.Zero, true))
            {
                engine.LoadScript(_script, -1);
                engine.Execute();
            }
        }
    }
}