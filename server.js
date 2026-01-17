const express = require('express');
const { exec } = require('child_process');
const path = require('path');
const app = express();
const port = 3000;

app.use(express.json());
app.use(express.static(path.join(__dirname, 'public')));
app.get('/', (req, res) => res.sendFile(path.join(__dirname, 'public', 'index.html')));

function executeCommand(command, res) {
    exec(command, (err, stdout, stderr) => {
        if (err) {
            console.error('Command error:', err);
            return res.status(500).send(stderr || err.message);
        }
        const output = (stdout + (stderr ? '\n' + stderr : '')).trim();
        res.send(output || 'OK');
    });
}

app.post('/addBook', (req, res) => {
    const { id, title, author, quantity } = req.body;
    const command = `"${path.join(__dirname, 'library.exe')}" addBook "${id}" "${title}" "${author}" ${quantity}`;
    executeCommand(command, res);
});

app.post('/deleteBook', (req, res) => {
    const { id, quantity } = req.body;
    const command = `"${path.join(__dirname, 'library.exe')}" deleteBook "${id}" ${quantity}`;
    executeCommand(command, res);
});

app.get('/displayBooks', (req, res) => {
    const command = `"${path.join(__dirname, 'library.exe')}" displayBooks`;
    executeCommand(command, res);
});

app.get('/countBooks', (req, res) => {
    const command = `"${path.join(__dirname, 'library.exe')}" countBooks`;
    executeCommand(command, res);
});

app.post('/borrowBook', (req, res) => {
    const { memberId, bookId } = req.body;
    const command = `"${path.join(__dirname, 'library.exe')}" borrowBook "${memberId}" "${bookId}"`;
    executeCommand(command, res);
});

app.post('/returnBook', (req, res) => {
    const { memberId, bookId } = req.body;
    const command = `"${path.join(__dirname, 'library.exe')}" returnBook "${memberId}" "${bookId}"`;
    executeCommand(command, res);
});

app.post('/addMember', (req, res) => {
    const { id, name, contact } = req.body;
    const command = `"${path.join(__dirname, 'library.exe')}" addMember "${id}" "${name}" "${contact}"`;
    executeCommand(command, res);
});

app.post('/deleteMember', (req, res) => {
    const { id } = req.body;
    const command = `"${path.join(__dirname, 'library.exe')}" deleteMember "${id}"`;
    executeCommand(command, res);
});

app.get('/displayMembers', (req, res) => {
    const command = `"${path.join(__dirname, 'library.exe')}" displayMembers`;
    executeCommand(command, res);
});

app.get('/searchBooks/:keyword', (req, res) => {
    const keyword = req.params.keyword;
    const command = `"${path.join(__dirname, 'library.exe')}" searchBooks "${keyword}"`;
    executeCommand(command, res);
});

app.get("/sort-books", (req, res) => {
    const attr = req.query.attribute;
    if (!attr || !['id', 'title', 'author'].includes(attr)) {
        return res.status(400).send('Invalid sort attribute');
    }
    
    const command = `"${path.join(__dirname, 'library.exe')}" sortBooks "${attr}"`;
    
    exec(command, (err, stdout, stderr) => {
        if (err) {
            console.error('Sort error:', err);
            return res.status(500).send('Sorting failed');
        }
        res.send('');
    });
});

app.get('/searchMembers/:keyword', (req, res) => {
    const keyword = req.params.keyword;
    const command = `"${path.join(__dirname, 'library.exe')}" searchMembers "${keyword}"`;
    executeCommand(command, res);
});

app.get("/sort-members", (req, res) => {
    const attr = req.query.attribute || "name";
    if (!['id', 'name', 'contact'].includes(attr)) {
        return res.status(400).send('Invalid sort attribute');
    }
    
    const command = `"${path.join(__dirname, 'library.exe')}" sortMembers "${attr}"`;
    
    exec(command, (err, stdout, stderr) => {
        if (err) {
            console.error('Sort error:', err);
            return res.status(500).send('Sorting failed');
        }
        res.send('');
    });
});

app.listen(port, () => console.log(`Server running at http://localhost:${port}`));