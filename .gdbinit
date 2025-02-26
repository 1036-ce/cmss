python
import gdb

workdir = "~/workspace/aflnet"
aflnet = workdir + "/aflnet" 
live555_testprogs = workdir + "/live555/testProgs"
in_dir = aflnet + "/tutorials/live555/in-rtsp"
out_dir = aflnet + "/out-live555"

network = "tcp://127.0.0.1/8554"
fuzzer_dir = aflnet + "/tutorials/live555/rtsp.dict"

other_args = " -P RTSP -D 10000 -q 3 -s 3 -E -K -R "

target_args = live555_testprogs + "/testOnDemandRTSPServer" + " 8554" 

run_args = "-d -i " + in_dir + " -o " + out_dir + " -N " + network + " -x " + fuzzer_dir + other_args + target_args

cmd = "set args " + run_args

print("run args: " + run_args)
gdb.execute(cmd)
end
