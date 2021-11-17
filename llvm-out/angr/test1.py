import angr
path = './obf'
base =0
project = angr.Project(path,auto_load_libs=False, load_options={ 
    'main_opts' : { 
    'custom_base_addr' : base 
    } 
})
initial_state = project.factory.entry_state()
simulation = project.factory.simgr(initial_state)
good = 0x011c9
simulation.explore(find=good) 
if simulation.found:
        solution_state = simulation.found[0]
       # solution = solution_state.posix.dumps(sys.stdin.fileno())
        solution = solution_state.posix.dumps(0)
        print("[+] Success! Solution is: {}".format(solution.decode("utf-8")))
        
        
        



