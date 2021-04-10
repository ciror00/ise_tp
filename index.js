const express = require("express");
const bodyParser = require('body-parser');
const app = express();
const mysql = require('mysql');

app.use(bodyParser.urlencoded({ extended: false }));
app.use(bodyParser.json());

var connection = mysql.createConnection({
  host     : 'localhost',
  user     : 'root',
  password : 'root',
  database : 'ise',
  port: '3307'
});

let medicion = {
 uuid:'',
 valor: ''
};

let respuesta = {
 error: false,
 codigo: 200,
 mensaje: '',
 response: []
};

// Ruta base de la API
app.get('/', function(req, res) {
 respuesta = {
  error: true,
  codigo: 200,
  mensaje: 'Endpoint base',
  response: []
 };
 res.send(respuesta);
});

// Ruta de las mediciones de la API
app.route('/mediciones')
    .get(function (req, res) { // Metodo GET para obtener las mediciones
        connection.query('SELECT * FROM mediciones WHERE 1', function (error, results, fields) {
            if (error) console.log('Error: ', error);
            //console.log(results[0]);
            respuesta = {
            error: false,
            codigo: 200,
            mensaje: 'lista de mediciones',
            response: results
            };
        });
        
        res.send(respuesta);
 })
    .post(function (req, res) { // Metodo POST para crear una medición
        if(!req.body.uuid || !req.body.valor) {
            respuesta = {
            error: true,
            codigo: 502,
            mensaje: 'El campo uuid y valor son requeridos',
            response: []
            };
        }else{
            var dml = 'INSERT INTO mediciones (uuid, valor) VALUES ('+req.body.uuid+','+req.body.valor+')';
            connection.query(dml, function (error, results, fields) {
                if (error) console.log('Error: ', error);
                respuesta = {
                error: false,
                codigo: 200,
                mensaje: 'medicion agregada exitosamente',
                response: results.affectedRows
                };
            });
        }
        res.send(respuesta);
    })
.put(function (req, res) { // Metodo PUT para actualizar registro
    if(!req.body.uuid || !req.body.valor || !req.body.id) {
        respuesta = {
        error: true,
        codigo: 502,
        mensaje: 'El campo id, uuid y valor son requeridos',
        response: []
        };
    }else{
        var dml = 'UPDATE mediciones SET valor = '+req.body.valor+' WHERE uuid = '+req.body.uuid+' AND id = '+req.body.id;
        connection.query(dml, function (error, results, fields) {
            if (error) console.log('Error: ', error);
            respuesta = {
            error: false,
            codigo: 200,
            mensaje: 'medicion actualizada exitosamente',
            response: results
            };
        });
    }
    res.send(respuesta);
 })
.delete(function (req, res) { // Metodo DELETE para borrar registro
    if(!req.body.id) {
        respuesta = {
        error: true,
        codigo: 502,
        mensaje: 'El campo id es requeridos',
        response: []
        };
    }else{
        var dml = 'DELETE FROM mediciones WHERE id =' + req.body.id;
        connection.query(dml, function (error, results, fields) {
            if (error) console.log('Error: ', error);
            respuesta = {
            error: false,
            codigo: 200,
            mensaje: 'medicion eliminada exitosamente',
            response: results
            };
        });
    }
    res.send(respuesta);
 });

 // Definicion para URLs inexistentes
app.use(function(req, res, next) {
 respuesta = {
  error: true, 
  codigo: 404, 
  mensaje: 'URL no encontrada'
 };
 res.status(404).send(respuesta);
});

// Inicializcacion de aplicacion
app.listen(3000, () => {
 console.log("El servidor está inicializado en el puerto 3000");
});