

These are my notes on how ODIN_II works and how I'm trying to complete assignment 2.

I have Verilog synthesis working in ODIN_II. If ODIN is segfaulting on -G or -A, you need to make sure there's a directory named OUTPUT next to ODIN that it can spit the output files into.

ODIN_II doesn't seem to actually support synthesizing multiple files.

I think I'd like to understand the BLIF a bit better first. I'm hoping they're similar to the way ODIN stores its graphs but I can't be sure.

Here's my game plan:
- Get ODIN synthesizing Verilog and get an LFSR working. (DONE)
- Figure out how abstract syntax trees are represented in memory.
- Figure out how netlists are represented in memory.
    - Perhaps use BLIF utilities to load simple .blif files into memory.
- Figure out how to load a blif so I can put my compiled LFSR into netlist form.
- Find where division is created in abstract syntax tree and fix it so it doesn't throw an error.
- Find where division is turned into a netlist.
- Insert my netlist in the place of the division netlist.



All the types are stored in types.h.

Found the reason why libargparse wouldn't compile in Xcode. I thought it was something super crazy obscure but it turns out <string> needed to be included in a single file :P

Ah, it turns out there's a lot more problems under the surface. Not even the ./verify_odin.sh script will run. Apparently it uses an operator that is newer than the version of bash on macos. I know I can upgrade bash, but the micro benchmarks fail on the first one anyways, so something else is immediately wrong too. It says there's an unexpected token. Its really a shame it won't work on my Mac. XCode is such a valuable tool in this situation. I'm not sure I'm going to be able to do this project without it.

QUESTIONS:

Why is the BLIF graph not made of logic gates or logic operations?
Answer: It is

LOCATIONS:
netlist_create_from_ast.cpp line 3852 (inside create_operation_node function) is where it outputs a division not supported error.
output_blif.cpp -> line 398 has some interesting stuff going on. I think this is only if you have hard nodes left that are undefined or something and they need special processing to output them. I could probably insert my BLIF at this point.
partial_map.cpp -> line 302

So apparently if I have the clk and rst congregated into one input then ODIN's loadBLIF doesn't like it. I might need to figure out how to get ODIN to insert my module using the division operator but separate the second input into two bits and hook them up to rst and clk.

define_mult_function doesn't seem to be called. I wonder if its only called if you have hard multipliers. If that's the case, I'm not interested in it.

The divide node starts out as a black box with multiple input pins and multiple output pins. The connections have to be properly transferred to the loaded BLIF which has a top level node for each input and output as I understand it.

A pin doesn't connect to another pin. A pin connects to another pin through a net. The process is different depending on whether we're talking about an input pin or an output pin.

An input pin stores what net it is connected to and stores its index in that net. It can disconnect itself by removing itself from the net using the index. You can call add_fanout_pin_to_net to add an input pin to an output net. The output needs a net because it can output to many pins.

/* assumes the pin spots have been allocated and the pin */
	net->fanout_pins = (npin_t**)vtr::realloc(net->fanout_pins, sizeof(npin_t*)*(net->num_fanout_pins+1));
	net->fanout_pins[net->num_fanout_pins] = pin;
	net->num_fanout_pins++;
	/* record the node and pin spot in the pin */
	pin->net = net;
	pin->pin_net_idx = net->num_fanout_pins-1;
	pin->type = INPUT;

An output pin is different. An output pin sets the driver pin of the net. I think any input connected to the net will be driven by net->driverPin.

Something is wrong with what I'm feeding ODIN because its incrementing the pin index when it really shouldn't between naming the same pin.

9/28/18 10:58pm I think I got it working!!!!

Now I need to prevent naming collisions somehow. I think renaming all the LFSR's nodes will work but I haven't figured out a way to cover all of them yet.
Done.

Now lets try simulating


Booya, it works, my dudes. :^)

ODIN is SUPER STRICT about the vector file specification. I had the wrong type of newlines in my files and it was just saying they didn't match.













