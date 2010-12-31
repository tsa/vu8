var io = loadmodule('io'),
    console = loadmodule('console')

var write1 = new io.FileWriter("punko")
var write2 = new io.FileWriter("bunko")

if (! write1.is_open()) {
    console.println("could not load file for write")
}
else {
    write1.println("hello friend")
    write2.println("hollo frond")
    write1.println("fond of the night")
    write2.println("frond frond of the night")
    write1.close()
    write2.close()
}

var reader = new io.FileReader("punko")
if (! reader.is_open()) {
    console.println("could not load file for read")
}
else {
    for (var line = reader.getln(); line; line = reader.getln()) {
        console.println("tata",line)
    }
}

