var file = loadmodule('file'),
    console = loadmodule('console')

var write1 = new file.Writer()
var write2 = new file.Writer("bunko")

if (! write1.open("punko") || ! write2.is_open()) {
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

var read1 = new file.Reader("punko")
if (! read1.is_open()) {
    console.println("could not load punko for read")
}
else for (var line = read1.getln(); line; line = read1.getln()) {
    console.println("tata",line)
}

var read2 = new file.Reader()
read2.open("bunko")
if (! read2.is_open()) {
    console.println("could not load bunko for read")
}
else for (var line = read2.getln(); line; line = read2.getln()) {
    console.println("papa",line)
}

console.println("exit")
